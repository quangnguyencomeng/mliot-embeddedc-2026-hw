// HW02: KIẾN TRÚC PHẦN MỀM VÀ QUẢN LÝ BỘ NHỚ SMART BIKE

#include <stdio.h>
#include <stdint.h>
#include <stddef.h> // Cung cấp hằng NULL cho việc kiểm tra con trỏ hàm

// TASK 1: STRUCT, UNION & BIT-FIELDS

typedef union {
    uint16_t raw_value;
    struct {
        // HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
        // Cú pháp "tên : số_bit" báo cho compiler biết field này rộng bao nhiêu bit.
        // Các field xếp theo thứ tự từ bit thấp (bit 0) đến bit cao (bit 15).
        uint16_t PWR_ON : 1;       // Bit [0]   : trạng thái nguồn xe
        uint16_t ASSIST_LEVEL : 2; // Bit [2:1] : mức trợ lực (0..3)
        uint16_t LIGHT_BRIGHT : 4; // Bit [6:3] : độ sáng đèn (0..15)
        uint16_t RESERVED : 9;     // Bit [15:7]: vùng dự trữ, chưa dùng
        // HỌC VIÊN KẾT THÚC VIẾT CODE
    } fields;
} Bike_Status_t;


// TASK 2: FUNCTION POINTERS & TABLE-DRIVEN ARCHITECTURE

void drive_eco(void) {
    printf("Bike Mode: ECO.\n");
}
void drive_normal(void) {
    printf("Bike Mode: NORMAL.\n");
}
void drive_sport(void) {
    printf("Bike Mode: SPORT.\n");
}

// HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
// Kiểu hàm: hàm không nhận tham số và không trả về giá trị.
typedef void (*DriveModeFn)(void);

// Mảng con trỏ hàm: index 0 -> ECO, 1 -> NORMAL, 2 -> SPORT.
static DriveModeFn drive_modes[3] = {
    drive_eco,
    drive_normal,
    drive_sport
};
// HỌC VIÊN KẾT THÚC VIẾT CODE


// TASK 3: ARCHITECTURE CALLBACKS

void Battery_Monitor(void (*overheat_cb)(void)) {
    int battery_temp = 45;

    // HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
    // Nếu nhiệt độ vượt ngưỡng an toàn thì gọi callback cảnh báo.
    if (battery_temp > 40) {
        // Kiểm tra con trỏ hàm trước khi gọi để tránh lỗi truy cập sai.
        if (overheat_cb != NULL) {
            overheat_cb(); // Gọi hàm xử lý khẩn cấp do bên ngoài truyền vào.
        }
    }
    // HỌC VIÊN KẾT THÚC VIẾT CODE
}

void Critical_Battery_Handler(void) {
    printf("[WARNING] Battery is overheated!\n");
}


// TASK 4: MEMORY MAP & CRASH LAB

const char BIKE_MODEL[] = "E-Bike X2026"; 
uint32_t total_odometer = 0;             

void crash_simulation(void) {
    // HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
    // volatile giúp compiler không tối ưu bỏ biến này đi.
    // Mỗi lần gọi hàm sẽ đẩy thêm 1 frame (chứa dummy[]) lên stack.
    // Đệ quy vô hạn -> stack đầy -> Stack Overflow.
    volatile int dummy[64];
    dummy[0] = 0;      // Ghi vào mảng để compiler không bỏ qua frame
    crash_simulation(); // Tự gọi lại chính nó, không có điều kiện dừng
    // HỌC VIÊN KẾT THÚC VIẾT CODE
}


// HÀM MAIN KIỂM TRA (Học viên giữ nguyên để chạy thử nghiệm)

int main() {
    Bike_Status_t my_bike;
    
    // 1. Test Task 1
    my_bike.raw_value = 0x0025; 
    printf("REGISTER STRUCTURE: \n");
    printf("PWR_ON: %d | ASSIST_LEVEL: %d | LIGHT_BRIGHT: %d\n\n", 
           my_bike.fields.PWR_ON, my_bike.fields.ASSIST_LEVEL, my_bike.fields.LIGHT_BRIGHT);

    // 2. Test Task 2
    printf("ENGINE CONTROLLING: \n");
    // HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
    // Lấy chỉ số_mode từ bit-field ASSIST_LEVEL đã giải mã ở Task 1.
    unsigned int mode_index = my_bike.fields.ASSIST_LEVEL;
    // Tính số phần tử trong mảng để kiểm tra biên an toàn.
    unsigned int table_size = sizeof(drive_modes) / sizeof(drive_modes[0]);
    // Nếu index hợp lệ mới gọi, nếu không thì in thông báo cảnh báo.
    if (mode_index < table_size) {
        drive_modes[mode_index](); // Gọi hàm qua con trỏ trong bảng
    } else {
        printf("Invalid drive mode index: %u\n", mode_index);
    }
    // HỌC VIÊN KẾT THÚC VIẾT CODE

    // 3. Test Task 3
    Battery_Monitor(Critical_Battery_Handler);
    printf("\n");

    // 4. Test Task 4
    int current_speed = 25; 
    printf("MEMORY ADDRESSES OF VARIABLES \n");
    printf("BIKE_MODEL (ROM/.rodata):   %p\n", (void*)BIKE_MODEL);
    printf("total_odometer (RAM/.bss):  %p\n", (void*)&total_odometer);
    printf("current_speed (RAM/Stack):  %p\n", (void*)&current_speed);

    // Bỏ comment dòng dưới để chạy thử bài Crash Lab
    // crash_simulation();

    return 0;
}