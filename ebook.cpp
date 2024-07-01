
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <istream>

using namespace std;

namespace request {
enum RequestType {
    READ,
    CHEER
};

struct RequestData {
    RequestType type;
    int id;
    optional<int> page_num;
};

RequestData ReadRequest(istream& input) {
    RequestData request_data;
    string req_name;
    input >> req_name >> request_data.id;
    if (req_name == "READ"s) {
        request_data.type = RequestType::READ;
        int page;
        input >> page;
        request_data.page_num = page;
    }
    else {
        request_data.type = RequestType::CHEER;
    }
    return request_data;
}
} // namespace request


namespace read_stats {

struct ReadingStat {
    // How many pages finshed by [index=id]-user
    vector<int> people_data = vector<int>(100'001);
    // How many users finished [i] pages
    vector<int> pages_data = vector<int>(1000);
};

// Возвращает долю людей, прочитавших меньше книг, чем пользователь с индексом id
double GetPartOfSlowerReaders(int id, const ReadingStat& stat) {
    // Если для данного пользователя пока не было ни одного события READ, 
    // доля считается равной 0
    double part = 0;
    if (stat.people_data[id] != 0) {
        // столько прочитал user
        int n_user_pages = stat.people_data[id];
        // случай когда пользователь прочитал лишь одну страницу:
        if (n_user_pages == 1) {
            part = 0;
        }
        else {
            // столько людей прочитали не меньше user 
            int n_people_read_not_less = stat.pages_data[n_user_pages] - 1;
            // если все люди прочитали меньше страниц, чем user
            if (n_people_read_not_less == 0) {
                part = 1;
            }
            else {
                // полное количество читающих людей, за исключением user
                int n_people_total = stat.pages_data[1] - 1;
                // количество людей, прочитавших страниц меньше, чем user:
                int n_people_read_less = n_people_total - n_people_read_not_less;
                part = static_cast<double>(n_people_read_less) / static_cast<double>(n_people_total);
            }
        }
    }
    return part;
}

// Обрабатывает оба типа запросов к читательской "базе" READ и CHEER
void ProcessRequest(const request::RequestData& request, ReadingStat& stat) {
    if (request.type == request::RequestType::READ) {
        // столько страниц уже было прочитано user-ом
        int n_pages_before_read = stat.people_data[request.id];
        // обновляем число прочитанных страниц
        stat.people_data[request.id] = request.page_num.value();
        // добавляем ещё одного пользователя для новых прочитанных user-ом страниц 
        for (int i = n_pages_before_read + 1; i <= request.page_num.value(); i++) {
            ++stat.pages_data[i];
        }
    }
    else if (request.type == request::RequestType::CHEER) {
        double part_read_less = GetPartOfSlowerReaders(request.id, stat);
        cout << setprecision(6) << part_read_less << endl;
    }
    else {
        throw std::logic_error("Unknown request type"s);
    }

    return;
}

}  // namespace read_stats

int main() {
    using namespace request;
    using namespace read_stats;
    
    ReadingStat read_stat;
    int n_requests;

    cin >> n_requests;

    for (int i = 0; i < n_requests; i++) {
        RequestData request = ReadRequest(cin);
        try {
            ProcessRequest(request, read_stat);
        }
        catch (const std::logic_error& err) {
            cout << err.what();
        }
    }
}

