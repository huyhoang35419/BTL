#include "AuthService.h"

KetQuaXacThuc AuthService::dangNhap(const std::string& username, const std::string& password) {
    KetQuaXacThuc kq;
    kq.thanhCong = false;

    auto nd = csdl->timNguoiDung(username);
    if (!nd) {
        kq.thongBao = "Tai khoan khong ton tai trong he thong!";
        return kq;
    }

    if (!nd->kiemTraMatKhau(password)) {
        kq.thongBao = "Mat khau khong chinh xac!";
        return kq;
    }

    if (nd->layTrangThai() == "CHUA_KICH_HOAT") {
        kq.thongBao = "Tai khoan chua duoc kich hoat qua OTP!";
        return kq;
    }

    if (nd->layTrangThai() == "BI_KHOA") {
        kq.thongBao = "Tai khoan da bi khoa boi Admin!";
        return kq;
    }

    kq.thanhCong = true;
    kq.thongBao = "Dang nhap thanh cong!";
    kq.nguoiDung = nd;
    kq.token = "TOKEN_" + nd->layMaNguoiDung() + "_" + std::to_string(std::time(nullptr));
    {
        std::lock_guard<std::mutex> lock(mutexToken);
        tokenNguoiDung[kq.token] = nd->layMaNguoiDung();
    }
    return kq;
}

std::shared_ptr<NguoiDung> AuthService::timNguoiDungTheoToken(const std::string& token) {
    std::lock_guard<std::mutex> lock(mutexToken);
    auto it = tokenNguoiDung.find(token);
    if (it == tokenNguoiDung.end()) return nullptr;
    auto nd = csdl->timNguoiDungTheoMa(it->second);
    if (!nd || nd->layTrangThai() != "KICH_HOAT") {
        tokenNguoiDung.erase(it);
        return nullptr;
    }
    return nd;
}

KetQuaXacThuc AuthService::dangKy(const std::string& username, const std::string& password,
                                 const std::string& hoTen, const std::string& email, const std::string& sdt) {
    KetQuaXacThuc kq;
    kq.thanhCong = false;

    if (username.empty() || password.empty() || email.empty()) {
        kq.thongBao = "Vui long nhap day du ten dang nhap, mat khau va email!";
        return kq;
    }

    if (csdl->timNguoiDung(username)) {
        kq.thongBao = "Ten dang nhap da ton tai tren he thong!";
        return kq;
    }

    if (csdl->timNguoiDungTheoEmail(email)) {
        kq.thongBao = "Dia chi email nay da duoc dang ky!";
        return kq;
    }

    std::string maND = csdl->sinhMaNguoiDung();
    // Mac dinh tai khoan moi tao la KhachHang voi trang thai CHUA_KICH_HOAT de cho OTP
    auto khachMoi = std::make_shared<KhachHang>(maND, username, password, hoTen, email, sdt, 0.0);
    khachMoi->datTrangThai("CHUA_KICH_HOAT");

    if (!csdl->themNguoiDung(khachMoi)) {
        kq.thongBao = "Khong the tao tai khoan!";
        return kq;
    }

    // Tao OTP va gui email
    std::string otp = OTPService::taoVaLuuOTP(email);
    EmailService::guiEmail(email, "Ma xac thuc OTP tai khoan WMS",
                           "Xin chao " + hoTen + ",\nMa OTP kich hoat tai khoan cua ban la: " + otp + "\nMa co hieu luc trong 5 phut.");

    kq.thanhCong = true;
    kq.thongBao = "Dang ky thanh cong! Vui long nhap ma OTP da duoc gui toi " + email + " de kich hoat.";
    kq.nguoiDung = khachMoi;
    return kq;
}

bool AuthService::xacThucTaiKhoanOTP(const std::string& email, const std::string& otp, std::string& thongBao) {
    auto nd = csdl->timNguoiDungTheoEmail(email);
    if (!nd) {
        thongBao = "Khong tim thay tai khoan voi email nay!";
        return false;
    }

    if (OTPService::xacThucOTP(email, otp)) {
        nd->datTrangThai("KICH_HOAT");
        csdl->luuXuongDia();
        thongBao = "Kich hoat tai khoan thanh cong! Ban co the dang nhap ngay.";
        return true;
    } else {
        thongBao = "Ma OTP khong dung hoac da het han (5 phut)!";
        return false;
    }
}

bool AuthService::yeuCauQuenMatKhau(const std::string& email, std::string& thongBao) {
    auto nd = csdl->timNguoiDungTheoEmail(email);
    if (!nd) {
        thongBao = "Khong tim thay tai khoan voi email: " + email;
        return false;
    }

    std::string otp = OTPService::taoVaLuuOTP(email);
    EmailService::guiEmail(email, "Yeu cau dat lai mat khau he thong WMS",
                           "Xin chao " + nd->layHoTen() + ",\nMa OTP de dat lai mat khau la: " + otp + "\nMa co hieu luc trong 5 phut.");

    thongBao = "Ma OTP dat lai mat khau da duoc gui vao email " + email;
    return true;
}

bool AuthService::datLaiMatKhau(const std::string& email, const std::string& otp, const std::string& matKhauMoi, std::string& thongBao) {
    auto nd = csdl->timNguoiDungTheoEmail(email);
    if (!nd) {
        thongBao = "Khong tim thay tai khoan voi email nay!";
        return false;
    }

    if (matKhauMoi.empty()) {
        thongBao = "Mat khau moi khong duoc de trong!";
        return false;
    }

    if (OTPService::xacThucOTP(email, otp)) {
        nd->datMatKhau(matKhauMoi);
        csdl->luuXuongDia();
        thongBao = "Dat lai mat khau thanh cong! Ban hay dang nhap lai.";
        return true;
    } else {
        thongBao = "Ma OTP khong chinh xac hoac da het han!";
        return false;
    }
}

