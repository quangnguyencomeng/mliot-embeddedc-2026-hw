// HW01: HỆ THỐNG GIÁM SÁT LÒ NHIỆT CÔNG NGHIỆP

#include <stdio.h>
#include <stdint.h>

// TASK 1: POINTERS & MEMORY

void parse_config(const uint8_t *config_packet, int16_t *high_threshold) {
    // HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
    if (config_packet ==NULL || high_threshold == NULL){
        return ;
    }
    *high_threshold = (int16_t)(config_packet[0] | (config_packet[1] << 8)); // << 8 là shift left 8
    // HỌC VIÊN KẾT THÚC VIẾT CODE
}

// TASK 2: COMPILER & VOLATILE

int16_t read_temperature_reg(void *hw_sensor_reg) {
    // HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
    if(hw_sensor_reg ==NULL) return 0;
    // để đảm bảo compiler không tối ưu hoá lệnh khi đặt trong loop, DO TRONG LOOP, KHI ĐỌC ĐI ĐỌC LẠI MỘT BIẾN, COMPILER CÓ THỂ ĐƯA NÓ VÀO CACHE ĐỂ LẤY LẠI XÀI CHO NHỮNG LẦN SAU MÀ KHÔNG CHECK LẠI BIẾN (CÓ THỂ LÀ BIẾN NÀY ĐƯỢC CẬP NHẬT LIÊN TỤC NÊN LÀ MỚI CẦN LÀM THẾ)
    // cần thêm volatile -> dùng để báo compiler phải đọc từ RAM/ register chứ không cache hay tối ưu bỏ qua lệnh đọc
    // DO hw_sensor_reg LÀ MỘT VOID POINTE (KHÔNG CÓ DATA TYPE), TRƯỚC KHI ĐỌC ĐƯỢC NÓ, CẦN CAST/ÉP NÓ VỀ MỘT DATA TYPE
        // ĐÓ LÀ LÍ DO MÀ TRƯỚC hw_sensor_reg cần cái này: (volatile int16_t*)hw_sensor_reg;
        // CẦN CAST NÓ CŨNG VỀ MỘT CON TRỎ -> CÓ * TRONG (volatile int16_t*)
    volatile int16_t *reg = (volatile int16_t*)hw_sensor_reg;
    //volatile int16_t *reg → khai báo reg là con trỏ trỏ tới volatile int16_t. Dấu * thuộc về tên biến reg, không phải kiểu.
    // (volatile int16_t *)hw_sensor_reg → cast ép kiểu void* thành volatile int16_t* 
    return *reg; // * ở đây là dereference đọc giá trị tại địa chỉ đó
    // HỌC VIÊN KẾT THÚC VIẾT CODE
}

// TASK 3: DATA TYPES & BITWISE OPERATIONS

void control_output(uint8_t *control_reg, uint8_t fan_enable, uint8_t alarm_enable) {
    // HỌC VIÊN BẮT ĐẦU VIẾT CODE TỪ ĐÂY
    if(control_reg ==NULL)return ;
    
    //BIT NÀO CẦN TƯƠNG TÁC THÌ MÌNH RESET BIT ĐÓ (ĐƯA VỀ 0) TRƯỚC -> KHÔNG QUAN TÂM GIÁ TRỊ NÓ NỮA VÌ MÌNH SẼ THAY ĐỔI GIÁ TRỊ CỦA NÓ TUỲ THUỘC VÀO CÁC BIẾN BOOL
    *control_reg &= ~(1<<0);
    *control_reg &= ~(1<<1);
    // (1<<0) HAY (1<<1) CÓ NGHĨA LÀ ĐẨY BIT CÓ GIÁ TRỊ 1 SANG VỊ TRÍ BIT 0/1
    // ~ LÀ NOT 
    // &= LÀ NHÂN BIT-WISE
    // VD DÃY 8-BIT 10110111:
            // (1<<0) LÀ DÃY 00000001
            // (1<<1) LÀ DÃY 00000010
        // ~ ĐẢO NGƯỢC LẠI 
            // ~(1<<0) LÀ 11111110
            // ~(1<<1) LÀ 11111101
        // &= DÔ 
            // 10110111 &= 11111110: 10110110
            // 10110111 &= 11111101: 10110101
    // LÀM NHƯ VẬY THÌ LÚC SAU ĐỔI GIÁ TRỊ DỄ KIỂM SOÁT

    if(fan_enable){
        *control_reg != (1<<0);
    }
    if (alarm_enable){
        *control_reg != (1<<1);
    }
    //TÍNH NĂNG TƯƠNG TỰ, CHỈ CÓ CÁI != THÌ LÀ CỘNG BIT-WISE
    // HỌC VIÊN KẾT THÚC VIẾT CODE
    
}

// HÀM MAIN KIỂM TRA (Học viên giữ nguyên để chạy thử nghiệm, chỉ thay đổi input nếu cần)

int main() {

    // 1. Test Task 1
    int16_t threshold = 0;
    uint8_t mock_packet[2] = {0x64, 0x00};
    parse_config(mock_packet, &threshold); // Có thể thay mock_packet thành NULL để test chống crash khi gặp NULL
    printf("Threshold Parsed: %d C\n", threshold);

    // 2. Test Task 2
    volatile int16_t mock_hardware_sensor = 105; 
    int16_t current_temp = read_temperature_reg((void*)&mock_hardware_sensor);
    printf("Current Temp Read: %d C\n", current_temp);

    // 3. Test Task 3
    uint8_t system_control_register = 0xFC; // 1111 1100
    if (current_temp >= threshold) {
        // Yêu cầu: fan = 1, alarm = 0
        control_output(&system_control_register, 1, 0);
    }
    printf("Control Reg Output: 0x%02X\n", system_control_register);

    return 0;
}