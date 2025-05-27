#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <cstdint>

using namespace std;

struct Request {
    string name;
    int count;
    uint32_t required_addresses;
    int subnet_mask;
    uint32_t start_ip;
};

uint32_t ip_to_int(const string &ip) {
    uint32_t a, b, c, d;
    char ch;
    istringstream iss(ip);
    iss >> a >> ch >> b >> ch >> c >> ch >> d;
    return (a << 24) | (b << 16) | (c << 8) | d;
}

string int_to_ip(uint32_t ip) {
    return to_string((ip >> 24) & 0xFF) + "." +
           to_string((ip >> 16) & 0xFF) + "." +
           to_string((ip >> 8) & 0xFF) + "." +
           to_string(ip & 0xFF);
}

uint32_t next_power_of_two(uint32_t n) {
    if (n == 0) return 1;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <base_cidr> <requests_file> <order>" << endl;
        return 1;
    }

    string base_cidr = argv[1];
    string requests_file = argv[2];
    string order = argv[3];

    // Parse base CIDR
    size_t slash_pos = base_cidr.find('/');
    if (slash_pos == string::npos) {
        cerr << "Invalid base CIDR format." << endl;
        return 1;
    }
    string base_ip_str = base_cidr.substr(0, slash_pos);
    int base_mask = stoi(base_cidr.substr(slash_pos + 1));
    if (base_mask < 0 || base_mask > 32) {
        cerr << "Invalid mask." << endl;
        return 1;
    }

    uint32_t base_ip = ip_to_int(base_ip_str);

    // Calculate network address
    uint32_t mask_bits;
    if (base_mask == 0) {
        mask_bits = 0;
    } else {
        mask_bits = 0xFFFFFFFF << (32 - base_mask);
    }
    uint32_t network_address = base_ip & mask_bits;

    uint64_t total_available = (base_mask == 0) ? (1ULL << 32) : (1ULL << (32 - base_mask));

    // Read requests
    ifstream in(requests_file);
    if (!in) {
        cerr << "Cannot open requests file." << endl;
        return 1;
    }

    vector<Request> requests;
    string line;
    while (getline(in, line)) {
        istringstream iss(line);
        string name;
        int count;
        if (!(iss >> name >> count)) {
            cerr << "Invalid request line: " << line << endl;
            return 1;
        }
        if (count <= 0) {
            cerr << "Invalid count: " << count << endl;
            return 1;
        }
        uint32_t required = next_power_of_two(count);
        int exponent = 0;
        uint32_t tmp = required;
        while (tmp >>= 1) {
            exponent++;
        }
        int subnet_mask = 32 - exponent;
        requests.push_back({name, count, required, subnet_mask, 0});
    }

    // Sort requests in descending order of required_addresses
    sort(requests.begin(), requests.end(), [](const Request &a, const Request &b) {
        return a.required_addresses > b.required_addresses;
    });

    // Check total required
    uint64_t sum_required = 0;
    for (const auto &req : requests) {
        sum_required += req.required_addresses;
    }
    if (sum_required > total_available) {
        cerr << "Insufficient address space." << endl;
        return 1;
    }

    // Allocate addresses
    if (order == "asc") {
        uint64_t current = network_address;
        for (auto &req : requests) {
            req.start_ip = static_cast<uint32_t>(current);
            current += req.required_addresses;
        }
    } else if (order == "desc") {
        uint64_t current = network_address + total_available;
        for (auto &req : requests) {
            current -= req.required_addresses;
            req.start_ip = static_cast<uint32_t>(current);
        }
    } else {
        cerr << "Invalid order. Use 'asc' or 'desc'." << endl;
        return 1;
    }

    // Output
    for (const auto &req : requests) {
        string network = int_to_ip(req.start_ip) + "/" + to_string(req.subnet_mask);
        cout << req.name << " " << req.required_addresses << " " << network << endl;
    }

    return 0;
}