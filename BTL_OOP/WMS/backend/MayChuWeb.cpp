#include "MayChuWeb.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Ham helper trich xuat gia tri chuoi tu JSON don gian
static std::string layGiaTriChuoiJSON(const std::string& json, const std::string& key) {
    std::string pattern = "\"" + key + "\"";
    size_t pos = json.find(pattern);
    if (pos == std::string::npos) return "";

    pos = json.find(':', pos);
    if (pos == std::string::npos) return "";

    // Tim ky tu dau cua gia tri (bo qua dau cach)
    pos = json.find_first_not_of(" \t\r\n", pos + 1);
    if (pos == std::string::npos) return "";

    if (json[pos] == '"') {
        size_t endPos = json.find('"', pos + 1);
        if (endPos != std::string::npos) {
            return json.substr(pos + 1, endPos - pos - 1);
        }
    } else {
        // So hoac boolean
        size_t endPos = json.find_first_of(",}\r\n \t", pos);
        if (endPos != std::string::npos) {
            return json.substr(pos, endPos - pos);
        } else {
            return json.substr(pos);
        }
    }
    return "";
}

static double layGiaTriSoJSON(const std::string& json, const std::string& key, double macDinh = 0.0) {
    std::string val = layGiaTriChuoiJSON(json, key);
    if (val.empty()) return macDinh;
    try {
        return std::stod(val);
    } catch (...) {
        return macDinh;
    }
}

// URL Decode
static std::string giaiMaURL(const std::string& src) {
    std::string res;
    char ch;
    int i, ii;
    for (i = 0; i < (int)src.length(); i++) {
        if (src[i] == '%') {
            sscanf(src.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            res += ch;
            i = i + 2;
        } else if (src[i] == '+') {
            res += ' ';
        } else {
            res += src[i];
        }
    }
    return res;
}

MayChuWeb::MayChuWeb(int port, const std::string& frontendDir)
    : congMang(port), socketLangNghe(INVALID_SOCKET), dangChay(false), thuMucFrontend(frontendDir) {
    csdl = CoSoDuLieu::layInstance();
    authService = std::make_shared<AuthService>(csdl);
    quanLyBanHang = std::make_shared<QuanLyBanHang>(csdl);
}

MayChuWeb::~MayChuWeb() {
    dung();
}

std::string MayChuWeb::layMimeType(const std::string& duongDan) {
    if (duongDan.ends_with(".html") || duongDan.ends_with(".htm")) return "text/html; charset=utf-8";
    if (duongDan.ends_with(".css")) return "text/css; charset=utf-8";
    if (duongDan.ends_with(".js")) return "application/javascript; charset=utf-8";
    if (duongDan.ends_with(".json")) return "application/json; charset=utf-8";
    if (duongDan.ends_with(".png")) return "image/png";
    if (duongDan.ends_with(".jpg") || duongDan.ends_with(".jpeg")) return "image/jpeg";
    if (duongDan.ends_with(".svg")) return "image/svg+xml";
    if (duongDan.ends_with(".ico")) return "image/x-icon";
    return "text/plain; charset=utf-8";
}

YeuCauHTTP MayChuWeb::phanTichYeuCau(const std::string& raw) {
    YeuCauHTTP req;
    std::istringstream stream(raw);
    std::string line;

    if (std::getline(stream, line)) {
        std::istringstream lineStream(line);
        lineStream >> req.phuongThuc >> req.duongDan;

        size_t qPos = req.duongDan.find('?');
        if (qPos != std::string::npos) {
            req.chuoiTruyVan = req.duongDan.substr(qPos + 1);
            req.duongDan = req.duongDan.substr(0, qPos);

            // Phan tich query string
            std::istringstream qStream(req.chuoiTruyVan);
            std::string pair;
            while (std::getline(qStream, pair, '&')) {
                size_t eqPos = pair.find('=');
                if (eqPos != std::string::npos) {
                    req.thamSo[pair.substr(0, eqPos)] = giaiMaURL(pair.substr(eqPos + 1));
                }
            }
        }
    }

    // Doc cac headers
    while (std::getline(stream, line) && line != "\r" && !line.empty()) {
        size_t cPos = line.find(':');
        if (cPos != std::string::npos) {
            std::string k = line.substr(0, cPos);
            std::string v = line.substr(cPos + 1);
            if (!v.empty() && v.back() == '\r') v.pop_back();
            while (!v.empty() && v.front() == ' ') v.erase(0, 1);
            req.tieuDe[k] = v;
        }
    }

    // Doc body
    std::string bodyContent;
    char ch;
    while (stream.get(ch)) {
        bodyContent += ch;
    }
    req.body = bodyContent;

    return req;
}

PhanHoiHTTP MayChuWeb::dinhTuyenYeuCau(const YeuCauHTTP& req) {
    if (req.phuongThuc == "OPTIONS") {
        return PhanHoiHTTP(200, "text/plain", "OK");
    }

    // 1. Dinh tuyen cac API
    if (req.duongDan == "/api/dang-nhap" && req.phuongThuc == "POST") return xuLyDangNhap(req);
    if (req.duongDan == "/api/dang-ky" && req.phuongThuc == "POST") return xuLyDangKy(req);
    if (req.duongDan == "/api/xac-thuc-otp" && req.phuongThuc == "POST") return xuLyXacThucOTP(req);
    if (req.duongDan == "/api/quen-mat-khau" && req.phuongThuc == "POST") return xuLyQuenMatKhau(req);
    if (req.duongDan == "/api/dat-lai-mat-khau" && req.phuongThuc == "POST") return xuLyDatLaiMatKhau(req);
    if (req.duongDan == "/api/lay-otp-hien-tai" && req.phuongThuc == "GET") return xuLyLayOTPHienTai(req);

    if (req.duongDan == "/api/nguoi-dung" && req.phuongThuc == "GET") return xuLyLayDanhSachNguoiDung(req);
    if (req.duongDan == "/api/nguoi-dung/them" && req.phuongThuc == "POST") return xuLyThemNguoiDung(req);
    if (req.duongDan == "/api/nguoi-dung/sua" && req.phuongThuc == "POST") return xuLySuaNguoiDung(req);
    if (req.duongDan == "/api/nguoi-dung/xoa" && req.phuongThuc == "POST") return xuLyXoaNguoiDung(req);

    if (req.duongDan == "/api/san-pham" && req.phuongThuc == "GET") return xuLyLayDanhSachSanPham(req);
    if (req.duongDan == "/api/san-pham/them" && req.phuongThuc == "POST") return xuLyThemSanPham(req);
    if (req.duongDan == "/api/san-pham/sua" && req.phuongThuc == "POST") return xuLySuaSanPham(req);
    if (req.duongDan == "/api/san-pham/xoa" && req.phuongThuc == "POST") return xuLyXoaSanPham(req);

    if (req.duongDan == "/api/kho/nhap" && req.phuongThuc == "GET") return xuLyLayPhieuNhap(req);
    if (req.duongDan == "/api/kho/nhap" && req.phuongThuc == "POST") return xuLyThemPhieuNhap(req);
    if (req.duongDan == "/api/kho/nhap/duyet" && req.phuongThuc == "POST") return xuLyDuyetPhieuNhap(req);

    if (req.duongDan == "/api/kho/kiem-ke" && req.phuongThuc == "GET") return xuLyLayKiemKe(req);
    if (req.duongDan == "/api/kho/kiem-ke" && req.phuongThuc == "POST") return xuLyThemKiemKe(req);

    if (req.duongDan == "/api/kho/canh-bao" && req.phuongThuc == "GET") return xuLyLayCanhBao(req);
    if (req.duongDan == "/api/kho/canh-bao/da-doc" && req.phuongThuc == "POST") return xuLyDocTatCaCanhBao(req);

    if (req.duongDan == "/api/hoa-don" && req.phuongThuc == "GET") return xuLyLayHoaDon(req);
    if (req.duongDan == "/api/don-hang/dat-hang" && req.phuongThuc == "POST") return xuLyDatHang(req);
    if (req.duongDan == "/api/don-hang/xac-nhan" && req.phuongThuc == "POST") return xuLyXacNhanDon(req);
    if (req.duongDan == "/api/don-hang/giao-hang" && req.phuongThuc == "POST") return xuLyGiaoHang(req);
    if (req.duongDan == "/api/don-hang/thanh-toan" && req.phuongThuc == "POST") return xuLyThanhToan(req);
    if (req.duongDan == "/api/don-hang/huy" && req.phuongThuc == "POST") return xuLyHuyDon(req);
    if (req.duongDan == "/api/don-hang/hoan-tac" && req.phuongThuc == "POST") return xuLyHoanTacHuy(req);

    if (req.duongDan == "/api/bao-cao" && req.phuongThuc == "GET") return xuLyBaoCao(req);

    // 2. Phuc vu Static Files
    std::string duongDanFile = req.duongDan;
    if (duongDanFile == "/" || duongDanFile.empty()) {
        duongDanFile = "/index.html";
    }

    std::string fullPath = thuMucFrontend + duongDanFile;
    std::ifstream file(fullPath, std::ios::binary);
    if (!file.is_open()) {
        fullPath = "../" + thuMucFrontend + duongDanFile;
        file.open(fullPath, std::ios::binary);
    }
    if (file.is_open()) {
        std::ostringstream ss;
        ss << file.rdbuf();
        return PhanHoiHTTP(200, layMimeType(duongDanFile), ss.str());
    }

    return PhanHoiHTTP(404, "text/html; charset=utf-8", "<h1>404 Not Found - WMS Web Server</h1>");
}

void MayChuWeb::xuLyKetNoi(SOCKET clientSocket) {
    std::vector<char> buffer(65536);
    int bytesReceived = recv(clientSocket, buffer.data(), (int)buffer.size() - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::string rawRequest(buffer.data(), bytesReceived);

        YeuCauHTTP req = phanTichYeuCau(rawRequest);
        PhanHoiHTTP resp = dinhTuyenYeuCau(req);

        std::ostringstream headerStream;
        headerStream << "HTTP/1.1 " << resp.maTrangThai << " " << (resp.maTrangThai == 200 ? "OK" : "Error") << "\r\n";
        headerStream << "Content-Type: " << resp.loaiNoiDung << "\r\n";
        headerStream << "Content-Length: " << resp.noiDung.size() << "\r\n";
        headerStream << "Access-Control-Allow-Origin: *\r\n";
        headerStream << "Access-Control-Allow-Methods: GET, POST, OPTIONS, PUT, DELETE\r\n";
        headerStream << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
        headerStream << "Connection: close\r\n\r\n";

        std::string headers = headerStream.str();
        send(clientSocket, headers.c_str(), (int)headers.size(), 0);
        send(clientSocket, resp.noiDung.c_str(), (int)resp.noiDung.size(), 0);
    }
    closesocket(clientSocket);
}

bool MayChuWeb::khoiDong() {
    WSADATA wsaData;
    int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsResult != 0) {
        std::cerr << "Loi khoi tao Winsock: " << wsResult << std::endl;
        return false;
    }

    socketLangNghe = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketLangNghe == INVALID_SOCKET) {
        std::cerr << "Loi tao socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }

    BOOL optval = TRUE;
    setsockopt(socketLangNghe, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));

    sockaddr_in diaChi;
    diaChi.sin_family = AF_INET;
    diaChi.sin_addr.s_addr = INADDR_ANY;
    diaChi.sin_port = htons(congMang);

    if (bind(socketLangNghe, (sockaddr*)&diaChi, sizeof(diaChi)) == SOCKET_ERROR) {
        std::cerr << "Loi bind port " << congMang << ": " << WSAGetLastError() << std::endl;
        closesocket(socketLangNghe);
        WSACleanup();
        return false;
    }

    if (listen(socketLangNghe, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Loi listen: " << WSAGetLastError() << std::endl;
        closesocket(socketLangNghe);
        WSACleanup();
        return false;
    }

    dangChay = true;
    std::cout << "==========================================================" << std::endl;
    std::cout << " MAY CHU WEB WMS DANG CHAY TAI: http://localhost:" << congMang << std::endl;
    std::cout << " Mau giao dien: Xanh nhat & Trang (Light Blue & White)" << std::endl;
    std::cout << " Mo trinh duyet va truy cap de bat dau su dung." << std::endl;
    std::cout << "==========================================================" << std::endl;

    while (dangChay) {
        SOCKET clientSocket = accept(socketLangNghe, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            if (!dangChay) break;
            continue;
        }
        std::thread([this, clientSocket]() {
            this->xuLyKetNoi(clientSocket);
        }).detach();
    }

    return true;
}

void MayChuWeb::dung() {
    dangChay = false;
    if (socketLangNghe != INVALID_SOCKET) {
        closesocket(socketLangNghe);
        socketLangNghe = INVALID_SOCKET;
    }
    WSACleanup();
}

// -------------------------------------------------------------
// CAC HAM XU LY REST API (API Handlers)
// -------------------------------------------------------------

PhanHoiHTTP MayChuWeb::xuLyDangNhap(const YeuCauHTTP& req) {
    std::string user = layGiaTriChuoiJSON(req.body, "tenDangNhap");
    std::string pass = layGiaTriChuoiJSON(req.body, "matKhau");

    auto kq = authService->dangNhap(user, pass);
    std::ostringstream ss;
    if (kq.thanhCong && kq.nguoiDung) {
        ss << "{\"thanhCong\":true,"
           << "\"thongBao\":\"" << kq.thongBao << "\","
           << "\"token\":\"" << kq.token << "\","
           << "\"nguoiDung\":" << kq.nguoiDung->xuatThongTinJSON()
           << "}";
        return PhanHoiHTTP(200, "application/json", ss.str());
    } else {
        ss << "{\"thanhCong\":false,\"thongBao\":\"" << kq.thongBao << "\"}";
        return PhanHoiHTTP(400, "application/json", ss.str());
    }
}

PhanHoiHTTP MayChuWeb::xuLyDangKy(const YeuCauHTTP& req) {
    std::string user = layGiaTriChuoiJSON(req.body, "tenDangNhap");
    std::string pass = layGiaTriChuoiJSON(req.body, "matKhau");
    std::string ten = layGiaTriChuoiJSON(req.body, "hoTen");
    std::string email = layGiaTriChuoiJSON(req.body, "email");
    std::string sdt = layGiaTriChuoiJSON(req.body, "soDienThoai");

    auto kq = authService->dangKy(user, pass, ten, email, sdt);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (kq.thanhCong ? "true" : "false")
       << ",\"thongBao\":\"" << kq.thongBao << "\"}";
    return PhanHoiHTTP(kq.thanhCong ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyXacThucOTP(const YeuCauHTTP& req) {
    std::string email = layGiaTriChuoiJSON(req.body, "email");
    std::string otp = layGiaTriChuoiJSON(req.body, "otp");
    std::string tb;

    bool ok = authService->xacThucTaiKhoanOTP(email, otp, tb);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << tb << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyQuenMatKhau(const YeuCauHTTP& req) {
    std::string email = layGiaTriChuoiJSON(req.body, "email");
    std::string tb;

    bool ok = authService->yeuCauQuenMatKhau(email, tb);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << tb << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyDatLaiMatKhau(const YeuCauHTTP& req) {
    std::string email = layGiaTriChuoiJSON(req.body, "email");
    std::string otp = layGiaTriChuoiJSON(req.body, "otp");
    std::string passMoi = layGiaTriChuoiJSON(req.body, "matKhauMoi");
    std::string tb;

    bool ok = authService->datLaiMatKhau(email, otp, passMoi, tb);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << tb << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyLayOTPHienTai(const YeuCauHTTP& req) {
    auto it = req.thamSo.find("email");
    std::string email = (it != req.thamSo.end()) ? it->second : "";
    std::string otp = OTPService::layOTPHienTai(email);

    std::ostringstream ss;
    ss << "{\"thanhCong\":true,\"email\":\"" << email << "\",\"otp\":\"" << otp << "\"}";
    return PhanHoiHTTP(200, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyLayDanhSachNguoiDung(const YeuCauHTTP& /*req*/) {
    auto list = csdl->layDanhSachNguoiDung();
    std::ostringstream ss;
    ss << "{\"thanhCong\":true,\"danhSach\":[";
    for (size_t i = 0; i < list.size(); ++i) {
        ss << list[i]->xuatThongTinJSON();
        if (i + 1 < list.size()) ss << ",";
    }
    ss << "]}";
    return PhanHoiHTTP(200, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyThemNguoiDung(const YeuCauHTTP& req) {
    std::string user = layGiaTriChuoiJSON(req.body, "tenDangNhap");
    std::string pass = layGiaTriChuoiJSON(req.body, "matKhau");
    std::string ten = layGiaTriChuoiJSON(req.body, "hoTen");
    std::string email = layGiaTriChuoiJSON(req.body, "email");
    std::string sdt = layGiaTriChuoiJSON(req.body, "soDienThoai");
    std::string vaiTro = layGiaTriChuoiJSON(req.body, "vaiTro");

    std::string maND = csdl->sinhMaNguoiDung();
    std::shared_ptr<NguoiDung> nd = nullptr;

    if (vaiTro == "ADMIN") {
        nd = std::make_shared<QuanTriVien>(maND, user, pass, ten, email, sdt);
    } else if (vaiTro == "QUAN_LY_KHO") {
        nd = std::make_shared<NguoiQuanLyKho>(maND, user, pass, ten, email, sdt);
    } else if (vaiTro == "BAN_HANG") {
        nd = std::make_shared<NhanVienBanHang>(maND, user, pass, ten, email, sdt);
    } else {
        nd = std::make_shared<KhachHang>(maND, user, pass, ten, email, sdt, 0.0);
    }

    bool ok = csdl->themNguoiDung(nd);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << (ok ? "Them nguoi dung thanh cong!" : "Ten dang nhap hoac email da ton tai!") << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLySuaNguoiDung(const YeuCauHTTP& req) {
    std::string ma = layGiaTriChuoiJSON(req.body, "maNguoiDung");
    std::string ten = layGiaTriChuoiJSON(req.body, "hoTen");
    std::string email = layGiaTriChuoiJSON(req.body, "email");
    std::string sdt = layGiaTriChuoiJSON(req.body, "soDienThoai");
    std::string pass = layGiaTriChuoiJSON(req.body, "matKhau");

    bool ok = csdl->capNhatNguoiDung(ma, ten, email, sdt, pass);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << (ok ? "Cap nhat thanh cong!" : "Khong tim thay nguoi dung!") << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyXoaNguoiDung(const YeuCauHTTP& req) {
    std::string ma = layGiaTriChuoiJSON(req.body, "maNguoiDung");
    bool ok = csdl->xoaNguoiDung(ma);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << (ok ? "Xoa tai khoan thanh cong!" : "Khong the xoa tai khoan!") << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyLayDanhSachSanPham(const YeuCauHTTP& /*req*/) {
    auto list = csdl->layDanhSachSanPham();
    std::ostringstream ss;
    ss << "{\"thanhCong\":true,\"danhSach\":[";
    for (size_t i = 0; i < list.size(); ++i) {
        ss << list[i]->xuatThongTinJSON();
        if (i + 1 < list.size()) ss << ",";
    }
    ss << "]}";
    return PhanHoiHTTP(200, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyThemSanPham(const YeuCauHTTP& req) {
    std::string ma = layGiaTriChuoiJSON(req.body, "maSP");
    std::string ten = layGiaTriChuoiJSON(req.body, "tenSP");
    std::string loai = layGiaTriChuoiJSON(req.body, "loaiSP");
    std::string dvt = layGiaTriChuoiJSON(req.body, "donViTinh");
    double gNhap = layGiaTriSoJSON(req.body, "giaNhap", 0);
    double gBan = layGiaTriSoJSON(req.body, "giaBan", 0);
    double slTon = layGiaTriSoJSON(req.body, "soLuongTon", 0);
    double minTon = layGiaTriSoJSON(req.body, "tonToiThieu", 5);
    double maxTon = layGiaTriSoJSON(req.body, "tonToiDa", 500);
    std::string viTri = layGiaTriChuoiJSON(req.body, "viTriKho");
    std::string moTa = layGiaTriChuoiJSON(req.body, "moTa");
    std::string nsx = layGiaTriChuoiJSON(req.body, "ngaySanXuat");
    std::string hsd = layGiaTriChuoiJSON(req.body, "hanSuDung");
    double trongLuong = layGiaTriSoJSON(req.body, "trongLuong", 1.0);
    double haoHut = layGiaTriSoJSON(req.body, "heSoHaoHut", 0.0);

    if (ma.empty()) {
        ma = "SP" + std::to_string(csdl->layDanhSachSanPham().size() + 1);
    }

    std::shared_ptr<SanPham> sp = nullptr;
    if (loai == "HAN_SU_DUNG") {
        sp = std::make_shared<SanPhamHanSuDung>(ma, ten, dvt, gNhap, gBan, slTon, nsx, hsd, minTon, maxTon, viTri, moTa);
    } else if (loai == "THEO_CAN") {
        sp = std::make_shared<SanPhamTheoCan>(ma, ten, dvt, gNhap, gBan, slTon, trongLuong, haoHut, minTon, maxTon, viTri, moTa);
    } else {
        sp = std::make_shared<SanPhamThuong>(ma, ten, dvt, gNhap, gBan, slTon, minTon, maxTon, viTri, moTa);
    }

    bool ok = csdl->themSanPham(sp);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << (ok ? "Them san pham thanh cong!" : "Ma san pham da ton tai!") << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLySuaSanPham(const YeuCauHTTP& req) {
    std::string ma = layGiaTriChuoiJSON(req.body, "maSP");
    std::string ten = layGiaTriChuoiJSON(req.body, "tenSP");
    std::string dvt = layGiaTriChuoiJSON(req.body, "donViTinh");
    double gNhap = layGiaTriSoJSON(req.body, "giaNhap");
    double gBan = layGiaTriSoJSON(req.body, "giaBan");
    double minTon = layGiaTriSoJSON(req.body, "tonToiThieu", 5);
    double maxTon = layGiaTriSoJSON(req.body, "tonToiDa", 500);
    std::string viTri = layGiaTriChuoiJSON(req.body, "viTriKho");
    std::string moTa = layGiaTriChuoiJSON(req.body, "moTa");
    std::string nsx = layGiaTriChuoiJSON(req.body, "ngaySanXuat");
    std::string hsd = layGiaTriChuoiJSON(req.body, "hanSuDung");

    bool ok = csdl->capNhatSanPham(ma, ten, dvt, gNhap, gBan, minTon, maxTon, viTri, moTa, nsx, hsd);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << (ok ? "Cap nhat san pham thanh cong!" : "Khong tim thay san pham!") << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyXoaSanPham(const YeuCauHTTP& req) {
    std::string ma = layGiaTriChuoiJSON(req.body, "maSP");
    bool ok = csdl->xoaSanPham(ma);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << (ok ? "Xoa san pham thanh cong!" : "Khong the xoa san pham!") << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyLayPhieuNhap(const YeuCauHTTP& /*req*/) {
    auto list = csdl->layDanhSachPhieuNhap();
    std::ostringstream ss;
    ss << "{\"thanhCong\":true,\"danhSach\":[";
    for (size_t i = 0; i < list.size(); ++i) {
        ss << list[i].xuatJSON();
        if (i + 1 < list.size()) ss << ",";
    }
    ss << "]}";
    return PhanHoiHTTP(200, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyThemPhieuNhap(const YeuCauHTTP& req) {
    std::string ncc = layGiaTriChuoiJSON(req.body, "nhaCungCap");
    std::string nguoiLap = layGiaTriChuoiJSON(req.body, "nguoiLapPhieu");
    std::string ghiChu = layGiaTriChuoiJSON(req.body, "ghiChu");
    std::string maSP = layGiaTriChuoiJSON(req.body, "maSP");
    double soLuong = layGiaTriSoJSON(req.body, "soLuong", 1.0);
    double donGiaNhap = layGiaTriSoJSON(req.body, "donGiaNhap", 0.0);
    std::string hsd = layGiaTriChuoiJSON(req.body, "hanSuDung");

    auto sp = csdl->timSanPhamTheoMa(maSP);
    if (!sp) {
        return PhanHoiHTTP(400, "application/json", "{\"thanhCong\":false,\"thongBao\":\"Khong tim thay san pham can nhap!\"}");
    }

    std::string maPhieu = csdl->sinhMaPhieuNhap();
    
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream timeSS;
    timeSS << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    PhieuNhapKho pn(maPhieu, timeSS.str(), ncc, nguoiLap, ghiChu);
    ChiTietNhapKho ct;
    ct.maChiTiet = "CTN_" + maPhieu;
    ct.maPhieu = maPhieu;
    ct.maSP = maSP;
    ct.tenSP = sp->layTenSP();
    ct.soLuong = soLuong;
    ct.donGiaNhap = (donGiaNhap > 0) ? donGiaNhap : sp->layGiaNhap();
    ct.thanhTien = ct.soLuong * ct.donGiaNhap;
    ct.hanSuDung = hsd;
    pn.themChiTiet(ct);
    pn.datTrangThai("CHO_DUYET");

    bool ok = csdl->themPhieuNhap(pn);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << (ok ? "Da gui phieu nhap cho quan ly duyet. Ton kho se cap nhat sau khi duoc phe duyet." : "Loi lap phieu nhap!") << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyDuyetPhieuNhap(const YeuCauHTTP& req) {
    auto authHeader = req.tieuDe.find("Authorization");
    const std::string prefix = "Bearer ";
    if (authHeader == req.tieuDe.end() || authHeader->second.rfind(prefix, 0) != 0) {
        return PhanHoiHTTP(401, "application/json", "{\"thanhCong\":false,\"thongBao\":\"Vui long dang nhap bang tai khoan quan tri.\"}");
    }
    auto nguoiDung = authService->timNguoiDungTheoToken(authHeader->second.substr(prefix.size()));
    if (!nguoiDung || !nguoiDung->coQuyenQuanTri()) {
        return PhanHoiHTTP(403, "application/json", "{\"thanhCong\":false,\"thongBao\":\"Chi quan tri vien duoc phe duyet phieu nhap.\"}");
    }

    const std::string maPhieu = layGiaTriChuoiJSON(req.body, "maPhieu");
    const bool ok = csdl->duyetPhieuNhap(maPhieu);
    const std::string thongBao = ok
        ? "Da duyet phieu nhap va cap nhat ton kho."
        : "Khong tim thay phieu cho duyet hoac phieu da duoc xu ly.";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json",
        std::string("{\"thanhCong\":") + (ok ? "true" : "false") + ",\"thongBao\":\"" + thongBao + "\"}");
}

PhanHoiHTTP MayChuWeb::xuLyLayKiemKe(const YeuCauHTTP& /*req*/) {
    auto list = csdl->layDanhSachKiemKe();
    std::ostringstream ss;
    ss << "{\"thanhCong\":true,\"danhSach\":[";
    for (size_t i = 0; i < list.size(); ++i) {
        ss << list[i].xuatJSON();
        if (i + 1 < list.size()) ss << ",";
    }
    ss << "]}";
    return PhanHoiHTTP(200, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyThemKiemKe(const YeuCauHTTP& req) {
    std::string maSP = layGiaTriChuoiJSON(req.body, "maSP");
    double tonThucTe = layGiaTriSoJSON(req.body, "tonThucTe");
    std::string nguoiKiemKe = layGiaTriChuoiJSON(req.body, "nguoiKiemKe");
    std::string lyDo = layGiaTriChuoiJSON(req.body, "lyDo");
    std::string cachXuLy = layGiaTriChuoiJSON(req.body, "cachXuLy");

    auto sp = csdl->timSanPhamTheoMa(maSP);
    if (!sp) {
        return PhanHoiHTTP(400, "application/json", "{\"thanhCong\":false,\"thongBao\":\"Khong tim thay san pham!\"}");
    }

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream timeSS;
    timeSS << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    PhieuKiemKe kk;
    kk.maKiemKe = csdl->sinhMaKiemKe();
    kk.ngayKiemKe = timeSS.str();
    kk.nguoiKiemKe = nguoiKiemKe;
    kk.maSP = maSP;
    kk.tenSP = sp->layTenSP();
    kk.tonSoSach = sp->laySoLuongTon();
    kk.tonThucTe = tonThucTe;
    kk.chenhLech = kk.tonThucTe - kk.tonSoSach;
    kk.lyDo = lyDo;
    kk.cachXuLy = cachXuLy;

    bool ok = csdl->themPhieuKiemKe(kk);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"Lap phieu kiem ke thanh cong! Da can bang ton kho.\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyLayCanhBao(const YeuCauHTTP& /*req*/) {
    csdl->kiemTraVaPhatCanhBaoTuDong();
    auto list = csdl->layDanhSachCanhBao();
    std::ostringstream ss;
    ss << "{\"thanhCong\":true,\"danhSach\":[";
    for (size_t i = 0; i < list.size(); ++i) {
        ss << list[i].xuatJSON();
        if (i + 1 < list.size()) ss << ",";
    }
    ss << "]}";
    return PhanHoiHTTP(200, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyDocTatCaCanhBao(const YeuCauHTTP& /*req*/) {
    csdl->danhDauDaDocCanhBao();
    return PhanHoiHTTP(200, "application/json", "{\"thanhCong\":true}");
}

PhanHoiHTTP MayChuWeb::xuLyLayHoaDon(const YeuCauHTTP& req) {
    auto authHeader = req.tieuDe.find("Authorization");
    const std::string prefix = "Bearer ";
    if (authHeader == req.tieuDe.end() || authHeader->second.rfind(prefix, 0) != 0) {
        return PhanHoiHTTP(401, "application/json", "{\"thanhCong\":false,\"thongBao\":\"Vui long dang nhap de xem don hang.\"}");
    }
    auto nguoiDung = authService->timNguoiDungTheoToken(authHeader->second.substr(prefix.size()));
    if (!nguoiDung || (!nguoiDung->coQuyenQuanTri() && !nguoiDung->coQuyenBanHang() && !nguoiDung->coQuyenMuaHang())) {
        return PhanHoiHTTP(403, "application/json", "{\"thanhCong\":false,\"thongBao\":\"Tai khoan khong co quyen xem don hang.\"}");
    }

    auto list = csdl->layDanhSachHoaDon();
    std::ostringstream ss;
    ss << "{\"thanhCong\":true,\"danhSach\":[";
    bool first = true;
    for (const auto& hoaDon : list) {
        if (nguoiDung->layVaiTro() == VaiTro::KHACH_HANG && hoaDon->layMaKhachHang() != nguoiDung->layMaNguoiDung()) continue;
        if (!first) ss << ",";
        ss << hoaDon->xuatJSON();
        first = false;
    }
    ss << "]}";
    return PhanHoiHTTP(200, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyDatHang(const YeuCauHTTP& req) {
    std::string maKH = layGiaTriChuoiJSON(req.body, "maKhachHang");
    std::string tenKH = layGiaTriChuoiJSON(req.body, "tenKhachHang");
    std::string sdt = layGiaTriChuoiJSON(req.body, "soDienThoai");
    std::string diaChi = layGiaTriChuoiJSON(req.body, "diaChiGiaoHang");
    std::string loaiGG = layGiaTriChuoiJSON(req.body, "loaiGiamGia");
    std::string loaiThue = layGiaTriChuoiJSON(req.body, "loaiThue");
    std::string ptTT = layGiaTriChuoiJSON(req.body, "phuongThucThanhToan");
    std::string ghiChu = layGiaTriChuoiJSON(req.body, "ghiChu");

    // Trich xuat danh sach mon hang: duoi dang maSP:soLuong, phan tach bang dau phay
    // Vi du: "danhSachMon": "SP01:2,SP03:1"
    std::string danhSachStr = layGiaTriChuoiJSON(req.body, "danhSachMon");
    std::vector<MonHangDatMua> dsMon;

    std::istringstream ssDs(danhSachStr);
    std::string item;
    while (std::getline(ssDs, item, ',')) {
        size_t cPos = item.find(':');
        if (cPos != std::string::npos) {
            MonHangDatMua mon;
            mon.maSP = item.substr(0, cPos);
            try {
                mon.soLuong = std::stod(item.substr(cPos + 1));
            } catch (...) {
                mon.soLuong = 1.0;
            }
            dsMon.push_back(mon);
        }
    }

    std::string maHDMoi, thongBaoLoi;
    bool ok = quanLyBanHang->taoDonHangMoi(maKH, tenKH, sdt, diaChi, dsMon, loaiGG, loaiThue, ptTT, ghiChu, maHDMoi, thongBaoLoi);

    std::ostringstream ss;
    if (ok) {
        ss << "{\"thanhCong\":true,\"maHoaDon\":\"" << maHDMoi << "\",\"thongBao\":\"Dat hang thanh cong! Don hang dang cho xac nhan.\"}";
        return PhanHoiHTTP(200, "application/json", ss.str());
    } else {
        ss << "{\"thanhCong\":false,\"thongBao\":\"" << thongBaoLoi << "\"}";
        return PhanHoiHTTP(400, "application/json", ss.str());
    }
}

PhanHoiHTTP MayChuWeb::xuLyXacNhanDon(const YeuCauHTTP& req) {
    std::string maHD = layGiaTriChuoiJSON(req.body, "maHoaDon");
    std::string nguoiXuLy = layGiaTriChuoiJSON(req.body, "nguoiXuLy");
    std::string thongBaoLoi;

    bool ok = quanLyBanHang->xacNhanDonHang(maHD, nguoiXuLy, thongBaoLoi);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << (ok ? "Xac nhan don hang thanh cong!" : thongBaoLoi) << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyGiaoHang(const YeuCauHTTP& req) {
    std::string maHD = layGiaTriChuoiJSON(req.body, "maHoaDon");
    std::string ttMoi = layGiaTriChuoiJSON(req.body, "trangThaiMoi");
    std::string thongBaoLoi;

    bool ok = quanLyBanHang->capNhatGiaoHang(maHD, ttMoi, thongBaoLoi);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << (ok ? "Cap nhat trang thai don hang thanh cong!" : thongBaoLoi) << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyThanhToan(const YeuCauHTTP& req) {
    std::string maHD = layGiaTriChuoiJSON(req.body, "maHoaDon");
    std::string thongBaoLoi;

    bool ok = quanLyBanHang->xacNhanThanhToan(maHD, thongBaoLoi);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << (ok ? "Xac nhan da thanh toan don hang!" : thongBaoLoi) << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyHuyDon(const YeuCauHTTP& req) {
    std::string maHD = layGiaTriChuoiJSON(req.body, "maHoaDon");
    std::string thongBaoLoi;

    // Ap dung Command Pattern
    bool ok = quanLyBanHang->huyDonHang(maHD, thongBaoLoi);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << (ok ? "Huy don hang thanh cong (Command Pattern da hoan ton kho an toan)!" : thongBaoLoi) << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyHoanTacHuy(const YeuCauHTTP& /*req*/) {
    std::string thongBaoLoi;
    // Undo Command Pattern
    bool ok = quanLyBanHang->hoanTacHuyDon(thongBaoLoi);
    std::ostringstream ss;
    ss << "{\"thanhCong\":" << (ok ? "true" : "false")
       << ",\"thongBao\":\"" << (ok ? "Hoan tac thanh cong (Command Undo)!" : thongBaoLoi) << "\"}";
    return PhanHoiHTTP(ok ? 200 : 400, "application/json", ss.str());
}

PhanHoiHTTP MayChuWeb::xuLyBaoCao(const YeuCauHTTP& /*req*/) {
    std::string jsonBaoCao = csdl->taoBaoCaoTongHopJSON();
    return PhanHoiHTTP(200, "application/json", jsonBaoCao);
}
