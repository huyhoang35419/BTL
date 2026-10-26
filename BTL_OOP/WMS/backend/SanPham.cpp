#include "SanPham.h"
#include <iostream>
#include <sstream>
#include <iomanip>

int SanPhamHanSuDung::tinhSoNgayConLai() const {
    if (hanSuDung.empty() || hanSuDung.length() < 10) return 999;
    try {
        int y = std::stoi(hanSuDung.substr(0, 4));
        int m = std::stoi(hanSuDung.substr(5, 2));
        int d = std::stoi(hanSuDung.substr(8, 2));

        std::tm tmHsd = {};
        tmHsd.tm_year = y - 1900;
        tmHsd.tm_mon = m - 1;
        tmHsd.tm_mday = d;

        std::time_t tHsd = std::mktime(&tmHsd);
        std::time_t tNow = std::time(nullptr);

        double seconds = std::difftime(tHsd, tNow);
        return (int)(seconds / (60 * 60 * 24));
    } catch (...) {
        return 999;
    }
}

bool SanPhamHanSuDung::daHetHan() const {
    return tinhSoNgayConLai() < 0;
}

bool SanPhamHanSuDung::sapHetHan(int nguongNgay) const {
    int ngay = tinhSoNgayConLai();
    return ngay >= 0 && ngay <= nguongNgay;
}

std::string SanPhamThuong::xuatThongTinJSON() const {
    std::ostringstream ss;
    ss << "{"
       << "\"maSP\":\"" << maSP << "\","
       << "\"tenSP\":\"" << tenSP << "\","
       << "\"loaiSP\":\"" << layTenLoai() << "\","
       << "\"donViTinh\":\"" << donViTinh << "\","
       << "\"giaNhap\":" << std::fixed << std::setprecision(0) << giaNhap << ","
       << "\"giaBan\":" << std::fixed << std::setprecision(0) << giaBan << ","
       << "\"soLuongTon\":" << soLuongTon << ","
       << "\"tonToiThieu\":" << tonToiThieu << ","
       << "\"tonToiDa\":" << tonToiDa << ","
       << "\"viTriKho\":\"" << viTriKho << "\","
       << "\"moTa\":\"" << moTa << "\","
       << "\"hinhAnh\":\"" << hinhAnh << "\","
       << "\"canhBaoTon\":" << (sapHetHang() ? "true" : "false")
       << "}";
    return ss.str();
}

std::string SanPhamHanSuDung::xuatThongTinJSON() const {
    std::ostringstream ss;
    int ngayConLai = tinhSoNgayConLai();
    ss << "{"
       << "\"maSP\":\"" << maSP << "\","
       << "\"tenSP\":\"" << tenSP << "\","
       << "\"loaiSP\":\"" << layTenLoai() << "\","
       << "\"donViTinh\":\"" << donViTinh << "\","
       << "\"giaNhap\":" << std::fixed << std::setprecision(0) << giaNhap << ","
       << "\"giaBan\":" << std::fixed << std::setprecision(0) << giaBan << ","
       << "\"soLuongTon\":" << soLuongTon << ","
       << "\"tonToiThieu\":" << tonToiThieu << ","
       << "\"tonToiDa\":" << tonToiDa << ","
       << "\"viTriKho\":\"" << viTriKho << "\","
       << "\"moTa\":\"" << moTa << "\","
       << "\"hinhAnh\":\"" << hinhAnh << "\","
       << "\"ngaySanXuat\":\"" << ngaySanXuat << "\","
       << "\"hanSuDung\":\"" << hanSuDung << "\","
       << "\"ngayConLai\":" << ngayConLai << ","
       << "\"daHetHan\":" << (daHetHan() ? "true" : "false") << ","
       << "\"sapHetHan\":" << (sapHetHan(7) ? "true" : "false") << ","
       << "\"canhBaoTon\":" << (sapHetHang() ? "true" : "false")
       << "}";
    return ss.str();
}

std::string SanPhamTheoCan::xuatThongTinJSON() const {
    std::ostringstream ss;
    ss << "{"
       << "\"maSP\":\"" << maSP << "\","
       << "\"tenSP\":\"" << tenSP << "\","
       << "\"loaiSP\":\"" << layTenLoai() << "\","
       << "\"donViTinh\":\"" << donViTinh << "\","
       << "\"giaNhap\":" << std::fixed << std::setprecision(0) << giaNhap << ","
       << "\"giaBan\":" << std::fixed << std::setprecision(0) << giaBan << ","
       << "\"soLuongTon\":" << soLuongTon << ","
       << "\"tonToiThieu\":" << tonToiThieu << ","
       << "\"tonToiDa\":" << tonToiDa << ","
       << "\"viTriKho\":\"" << viTriKho << "\","
       << "\"moTa\":\"" << moTa << "\","
       << "\"hinhAnh\":\"" << hinhAnh << "\","
       << "\"trongLuong\":" << trongLuongMacDinh << ","
       << "\"heSoHaoHut\":" << heSoHaoHut << ","
       << "\"canhBaoTon\":" << (sapHetHang() ? "true" : "false")
       << "}";
    return ss.str();
}

