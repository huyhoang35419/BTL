#ifndef QUANLYKHO_H
#define QUANLYKHO_H

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "SanPham.h"
#include "Observer.h"

// Dong chi tiet trong phieu nhap kho
struct ChiTietNhapKho {
    std::string maChiTiet;
    std::string maPhieu;
    std::string maSP;
    std::string tenSP;
    double soLuong;
    double donGiaNhap;
    double thanhTien;
    std::string hanSuDung;

    std::string xuatJSON() const {
        std::ostringstream ss;
        ss << "{"
           << "\"maChiTiet\":\"" << maChiTiet << "\","
           << "\"maPhieu\":\"" << maPhieu << "\","
           << "\"maSP\":\"" << maSP << "\","
           << "\"tenSP\":\"" << tenSP << "\","
           << "\"soLuong\":" << soLuong << ","
           << "\"donGiaNhap\":" << std::fixed << std::setprecision(0) << donGiaNhap << ","
           << "\"thanhTien\":" << std::fixed << std::setprecision(0) << thanhTien << ","
           << "\"hanSuDung\":\"" << hanSuDung << "\""
           << "}";
        return ss.str();
    }
};

// Phieu nhap kho tu Nha Cung Cap (PO)
class PhieuNhapKho {
private:
    std::string maPhieu;
    std::string ngayNhap;
    std::string nhaCungCap;
    std::string nguoiLapPhieu;
    double tongTien;
    std::string ghiChu;
    std::string trangThai;
    std::vector<ChiTietNhapKho> danhSachChiTiet;

public:
    PhieuNhapKho(const std::string& ma, const std::string& ngay, const std::string& ncc,
                 const std::string& nguoiLap, const std::string& note = "")
        : maPhieu(ma), ngayNhap(ngay), nhaCungCap(ncc), nguoiLapPhieu(nguoiLap),
          tongTien(0.0), ghiChu(note), trangThai("DA_DUYET") {}

    std::string layMaPhieu() const { return maPhieu; }
    std::string layNgayNhap() const { return ngayNhap; }
    std::string layNhaCungCap() const { return nhaCungCap; }
    std::string layNguoiLapPhieu() const { return nguoiLapPhieu; }
    double layTongTien() const { return tongTien; }
    std::string layGhiChu() const { return ghiChu; }
    std::string layTrangThai() const { return trangThai; }
    void datTrangThai(const std::string& tt) { trangThai = tt; }
    const std::vector<ChiTietNhapKho>& layDanhSachChiTiet() const { return danhSachChiTiet; }

    void themChiTiet(const ChiTietNhapKho& ct) {
        danhSachChiTiet.push_back(ct);
        tongTien += ct.thanhTien;
    }

    std::string xuatJSON() const {
        std::ostringstream ss;
        ss << "{"
           << "\"maPhieu\":\"" << maPhieu << "\","
           << "\"ngayNhap\":\"" << ngayNhap << "\","
           << "\"nhaCungCap\":\"" << nhaCungCap << "\","
           << "\"nguoiLapPhieu\":\"" << nguoiLapPhieu << "\","
           << "\"tongTien\":" << std::fixed << std::setprecision(0) << tongTien << ","
           << "\"ghiChu\":\"" << ghiChu << "\","
           << "\"trangThai\":\"" << trangThai << "\","
           << "\"danhSachChiTiet\":[";
        for (size_t i = 0; i < danhSachChiTiet.size(); ++i) {
            ss << danhSachChiTiet[i].xuatJSON();
            if (i + 1 < danhSachChiTiet.size()) ss << ",";
        }
        ss << "]}";
        return ss.str();
    }
};

// Phieu kiem ke kho (Doi chieu thuc te & so sach)
struct PhieuKiemKe {
    std::string maKiemKe;
    std::string ngayKiemKe;
    std::string nguoiKiemKe;
    std::string maSP;
    std::string tenSP;
    double tonSoSach;
    double tonThucTe;
    double chenhLech; // thucTe - soSach
    std::string lyDo;
    std::string cachXuLy; // DIEU_CHINH_TANG, DIEU_CHINH_GIAM, GHI_NHAN_HU_HAI

    std::string xuatJSON() const {
        std::ostringstream ss;
        ss << "{"
           << "\"maKiemKe\":\"" << maKiemKe << "\","
           << "\"ngayKiemKe\":\"" << ngayKiemKe << "\","
           << "\"nguoiKiemKe\":\"" << nguoiKiemKe << "\","
           << "\"maSP\":\"" << maSP << "\","
           << "\"tenSP\":\"" << tenSP << "\","
           << "\"tonSoSach\":" << tonSoSach << ","
           << "\"tonThucTe\":" << tonThucTe << ","
           << "\"chenhLech\":" << chenhLech << ","
           << "\"lyDo\":\"" << lyDo << "\","
           << "\"cachXuLy\":\"" << cachXuLy << "\""
           << "}";
        return ss.str();
    }
};

#endif // QUANLYKHO_H

