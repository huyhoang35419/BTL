#include "QuanLyBanHang.h"
#include "Command.cpp" // De dung LenhHuyHoaDon
#include <chrono>
#include <ctime>
#include <iomanip>

static std::string layNgayGioHienTai() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool QuanLyBanHang::taoDonHangMoi(const std::string& maKH, const std::string& tenKH, const std::string& sdt,
                                  const std::string& diaChi, const std::vector<MonHangDatMua>& danhSachMon,
                                  const std::string& loaiGiamGia, const std::string& loaiThue,
                                  const std::string& phuongThucTT, const std::string& ghiChu,
                                  std::string& maHDMoi, std::string& thongBaoLoi) {
    if (danhSachMon.empty()) {
        thongBaoLoi = "Gio hang dang trong, khong the dat hang!";
        return false;
    }

    if (tenKH.empty() || sdt.empty()) {
        thongBaoLoi = "Vui long nhap day du ten nguoi nhan va so dien thoai!";
        return false;
    }

    maHDMoi = csdl->sinhMaHoaDon();
    auto hd = std::make_shared<HoaDon>(
        maHDMoi, layNgayGioHienTai(), maKH, tenKH, sdt, diaChi, phuongThucTT,
        "CHO_XAC_NHAN", (phuongThucTT == "CHUYEN_KHOAN" ? "CHUA_THANH_TOAN" : "CHUA_THANH_TOAN"),
        "Online-Web", ghiChu);

    int demCT = 1;
    for (const auto& mon : danhSachMon) {
        auto sp = csdl->timSanPhamTheoMa(mon.maSP);
        if (!sp) {
            thongBaoLoi = "Khong tim thay san pham co ma " + mon.maSP;
            return false;
        }

        std::string loiKho;
        if (!sp->kiemTraXuatKho(mon.soLuong, loiKho)) {
            thongBaoLoi = "San pham " + sp->layTenSP() + ": " + loiKho;
            return false;
        }

        // Tinh don gia tai thoi diem ban
        double donGia = sp->tinhGiaBanThucTe(1.0);
        std::string maCT = "CT" + std::to_string(demCT++) + "_" + maHDMoi;
        ChiTietHoaDon ct(maCT, maHDMoi, sp->layMaSP(), sp->layTenSP(), mon.soLuong, donGia);
        hd->themChiTiet(ct);
    }

    // Ap dung Strategy giam gia & thue
    auto clGiamGia = BoChonChienLuoc::taoChienLuocGiamGia(loaiGiamGia);
    auto clThue = BoChonChienLuoc::taoChienLuocThue(loaiThue);
    hd->apDungChienLuoc(clGiamGia, clThue);

    if (!csdl->themHoaDon(hd)) {
        thongBaoLoi = "Loi ton kho hoac khong the luu hoa don!";
        return false;
    }

    return true;
}

bool QuanLyBanHang::xacNhanDonHang(const std::string& maHD, const std::string& nguoiXuLy, std::string& thongBaoLoi) {
    auto hd = csdl->timHoaDonTheoMa(maHD);
    if (!hd) {
        thongBaoLoi = "Khong tim thay don hang " + maHD;
        return false;
    }
    if (hd->layTrangThaiDon() != "CHO_XAC_NHAN") {
        thongBaoLoi = "Don hang da o trang thai: " + hd->layTrangThaiDon();
        return false;
    }

    return csdl->capNhatTrangThaiHoaDon(maHD, "DA_XAC_NHAN", "", nguoiXuLy);
}

bool QuanLyBanHang::capNhatGiaoHang(const std::string& maHD, const std::string& trangThaiMoi, std::string& thongBaoLoi) {
    auto hd = csdl->timHoaDonTheoMa(maHD);
    if (!hd) {
        thongBaoLoi = "Khong tim thay don hang " + maHD;
        return false;
    }

    std::string ttThanhToan = "";
    if (trangThaiMoi == "DA_GIAO") {
        // Khi da giao hang thanh cong, neu thu tien mat thi danh dau da thanh toan
        ttThanhToan = "DA_THANH_TOAN";
    }

    return csdl->capNhatTrangThaiHoaDon(maHD, trangThaiMoi, ttThanhToan, "");
}

bool QuanLyBanHang::xacNhanThanhToan(const std::string& maHD, std::string& thongBaoLoi) {
    auto hd = csdl->timHoaDonTheoMa(maHD);
    if (!hd) {
        thongBaoLoi = "Khong tim thay don hang " + maHD;
        return false;
    }
    return csdl->capNhatTrangThaiHoaDon(maHD, "", "DA_THANH_TOAN", "");
}

bool QuanLyBanHang::huyDonHang(const std::string& maHD, std::string& thongBaoLoi) {
    auto lenhHuy = std::make_shared<LenhHuyHoaDon>(maHD, csdl);
    return quanLyLenh.thucThiLenh(lenhHuy, thongBaoLoi);
}

bool QuanLyBanHang::hoanTacHuyDon(std::string& thongBaoLoi) {
    return quanLyLenh.hoanTacLenhGanNhat(thongBaoLoi);
}

