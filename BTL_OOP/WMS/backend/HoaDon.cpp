#include "HoaDon.h"

std::string HoaDon::xuatJSON() const {
    std::ostringstream ss;
    ss << "{"
       << "\"maHoaDon\":\"" << maHoaDon << "\","
       << "\"ngayTao\":\"" << ngayTao << "\","
       << "\"maKhachHang\":\"" << maKhachHang << "\","
       << "\"tenKhachHang\":\"" << tenKhachHang << "\","
       << "\"soDienThoai\":\"" << soDienThoai << "\","
       << "\"diaChiGiaoHang\":\"" << diaChiGiaoHang << "\","
       << "\"tongTienGoc\":" << std::fixed << std::setprecision(0) << tongTienGoc << ","
       << "\"loaiGiamGia\":\"" << loaiGiamGia << "\","
       << "\"tienGiamGia\":" << std::fixed << std::setprecision(0) << tienGiamGia << ","
       << "\"loaiThue\":\"" << loaiThue << "\","
       << "\"tienThue\":" << std::fixed << std::setprecision(0) << tienThue << ","
       << "\"tongTienThanhToan\":" << std::fixed << std::setprecision(0) << tongTienThanhToan << ","
       << "\"phuongThucThanhToan\":\"" << phuongThucThanhToan << "\","
       << "\"trangThaiDon\":\"" << trangThaiDon << "\","
       << "\"trangThaiThanhToan\":\"" << trangThaiThanhToan << "\","
       << "\"nguoiXuLy\":\"" << nguoiXuLy << "\","
       << "\"ghiChu\":\"" << ghiChu << "\","
       << "\"danhSachChiTiet\":[";

    for (size_t i = 0; i < danhSachChiTiet.size(); ++i) {
        ss << danhSachChiTiet[i].xuatJSON();
        if (i + 1 < danhSachChiTiet.size()) {
            ss << ",";
        }
    }
    ss << "]}";
    return ss.str();
}

