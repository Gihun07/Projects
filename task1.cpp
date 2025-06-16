#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <stdexcept>

using namespace std;

// Các thành phần của sensor
struct sensor {
    int id;
    string time;
    double value;
};

// Hàm lấy thời gian
string realtime(time_t time) {
    tm* ltm = localtime(&time);
    ostringstream oss; // ghi lại chuỗi
    oss << put_time(ltm, "%Y:%m:%d %H:%M:%S"); // dấu thời gian đo phải lấy theo format (YYYY:MM:DD hh:mm:ss)
    return oss.str(); // chuyển sang kiểu dữ liệu string
}

// Hàm tạo ra kết quả ngẫu nhiên
double sensorresult() {
    double res = ((double)rand() / RAND_MAX) * 800; // tạo ra các giá trị ngẫu nhiên trong khoảng từ 0-800
    return res;
}

// Hàm chạy trên cmd
bool cmdbuild1(int argc, char *argv[], int& sensornumber, int& samplingtime, int& interval) {
    sensornumber = 1; // số lượng cảm biến mặc định là 1
    samplingtime = 60; // thời gian lấy mẫu mặc định là 60s
    interval = 24; // khoảng thời gian mặc định là 24h
    bool nflag = false, stflag = false, siflag = false, check = false;

    // kiểm tra câu lệnh người dùng nhập
    for (int i = 1; i < argc; i++) { // bắt đầu từ i=1 bởi vì argv[0] là tên chương trình
        string agv = argv[i];

        if (agv == "-n" && i < argc - 1) { // kiểm tra xem câu lệnh cmd có "-n" hay không
            try {
                sensornumber = stoi(argv[++i]); // chuyển sang kiểu dữ liệu int
                nflag = true;
            } catch (const invalid_argument& e) {
                cerr << "Error 01: Invalid number for -n parameter!" << endl;
                check = true;
            }
        }

        // cần có câu lệnh "i < argc-1" để tránh truy cập một phần tử không tồn tại, gây ra lỗi chương trình

        else if (agv == "-st" && i < argc - 1) { // kiểm tra xem câu lệnh cmd có "-st" hay không
            try {
                samplingtime = stoi(argv[++i]); // chuyển sang kiểu dữ liệu int
                stflag = true;
            } catch (const invalid_argument& e) {
                cerr << "Error 01: Invalid number for -st parameter!" << endl;
                check = true;
            }
        }

        else if (agv == "-si" && i < argc - 1) { // kiểm tra xem câu lệnh cmd có "-si" hay không
            try {
                interval = stoi(argv[++i]); // chuyển sang kiểu dữ liệu int
                siflag = true;
            } catch (const invalid_argument& e) {
                cerr << "Error 01: Invalid number for -si parameter!" << endl;
                check = true;
            }
        }

        else {
            cerr << "Error 01: Invalid command!" << endl;
            check = true;
        }
    }

    // kiểm tra giá trị người dùng nhập
    if (sensornumber < 1) {
        cerr << "Error 02: Number of sensor(s) must be a positive integer!" << endl;
        check = true;
    }

    if (samplingtime < 10) {
        cerr << "Error 04: The smallest sampling time allowed is 10 second!" << endl;
        check = true;
    }

    if (interval < 1) {
        cerr << "Error 05: The smallest duration allowed is 1 hour!" << endl;
        check = true;
    }

    // kiểm tra xem file có ở trạng thái read-only hay không
    ofstream testfile("dust_sensor.csv", ios::app);
    if (!testfile.is_open()) {
        cerr << "Error 03: dust_sensor.csv access denied" << endl;
        check = true;
    }
    testfile.close();

    // nếu người dùng không nhập gì, dùng giá trị mặc định
    if (!check) {
        if (!nflag)  cout << "Using default number of sensor: " << sensornumber << endl;
        if (!stflag) cout << "Using default sampling time: " << samplingtime << " seconds" << endl;
        if (!siflag) cout << "Using default interval: " << interval << " hours" << endl;
    }
    
    return !check; // trả về true nếu không có lỗi
}

// Hàm tạo file csv
void datatable1(int sensornumber, int samplingtime, int interval) {
    time_t currenttime = time(0); // lấy thời gian thực của hệ thống
    time_t starttime = currenttime - (interval * 3600); // thời gian bắt đầu sớm hơn "interval" giờ so với thời gian nhập

    ofstream file("dust_sensor.csv"); // bảng số liệu lưu vào file "dust_sensor.csv"
    if (!file.is_open()) {
        cerr << "Error: Cannot create dust_sensor.csv file!" << endl;
        return;
    }
    
    file << "id,time,value" << endl; // header line

    time_t simulationtime = starttime; // cần tạo một biến mới giống giá trị "starttime" ban đầu

    while (simulationtime <= currenttime) {
        string currentsampletime = realtime(simulationtime); // thời gian lấy mẫu cho tất cả cảm biến trong một vòng lặp giống nhau

        for (int i = 1; i <= sensornumber; i++) {
            sensor data;
            data.id = i; // lấy id sensor
            data.time = currentsampletime; // lấy giá trị thời gian sensor
            data.value = sensorresult(); // lấy giá trị kết quả sensor

            // ghi lại vào file csv
            file << data.id << "," << data.time << "," << setprecision(1) << fixed << data.value << endl; // kết quả với độ chia 0.1
        }

        // cập nhật thời gian sau khi hoàn thành tất cả cảm biến
        simulationtime += samplingtime;
    }
    cout << "Simulation data has saved to dust_sensor.csv" << endl;
    file.close();
}

int main(int argc, char *argv[]) {
    srand(time(0)); // khởi tạo seed cho random number generator
    
    int sensornumber, samplingtime, interval;

    // thoát ra nếu như command line bị nhập sai
    if (!cmdbuild1(argc, argv, sensornumber, samplingtime, interval)) {
        return 1;
    }

    if (sensornumber == 1 && samplingtime == 60 && interval == 24) {
        cout << " " << endl;
    }
    else {
        
        cout << "Number of sensors: " << sensornumber << endl;
        cout << "Sampling time: " << samplingtime << " seconds" << endl;
        cout << "Simulation interval: " << interval << " hours" << endl;
        cout << " " << endl;
    }

    datatable1(sensornumber, samplingtime, interval);
    return 0;
}