#include "CoSoDuLieu.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <chrono>
#include <iomanip>

std::shared_ptr<CoSoDuLieu> CoSoDuLieu::instance = nullptr;
std::mutex CoSoDuLieu::mutexKhoa;

static std::string layThoiGianHienTai() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

static std::string layNgayHienTai() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d");
    return ss.str();
}

CoSoDuLieu::CoSoDuLieu(const std::string& dbPath)
    : duongDanFileDB(dbPath), demCanhBao(10), demHoaDon(10), demPhieuNhap(10), demKiemKe(10), demNguoiDung(10) {
}

std::shared_ptr<CoSoDuLieu> CoSoDuLieu::layInstance(const std::string& dbPath) {
    std::lock_guard<std::mutex> lock(mutexKhoa);
    if (!instance) {
        instance = std::shared_ptr<CoSoDuLieu>(new CoSoDuLieu(dbPath));
        instance->khoiTao();
    }
    return instance;
}

void CoSoDuLieu::khoiTao() {
    std::ifstream f(duongDanFileDB);
    if (!f.good()) {
        std::string fallbackPath = "../" + duongDanFileDB;
        std::ifstream f2(fallbackPath);
        if (f2.good()) {
            duongDanFileDB = fallbackPath;
            f2.close();
        }
    } else {
        f.close();
    }

    std::ifstream fCheck(duongDanFileDB);
    if (fCheck.good()) {
        fCheck.close();
        docTuDia();
    } else {
        napDuLieuMau();
        luuXuongDia();
    }
    kiemTraVaPhatCanhBaoTuDong();
}

void CoSoDuLieu::napDuLieuMau() {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    danhSachNguoiDung.clear();
    danhSachSanPham.clear();
    danhSachHoaDon.clear();
    danhSachPhieuNhap.clear();
    danhSachKiemKe.clear();
    danhSachCanhBao.clear();

    // 1. Nguoi dung mau
    danhSachNguoiDung.push_back(std::make_shared<QuanTriVien>(
        "ND01", "admin", "admin123", "Quan Tri Vien He Thong", "admin@wms.com", "0901234567"));
    danhSachNguoiDung.push_back(std::make_shared<NguoiQuanLyKho>(
        "ND02", "thukho", "kho123", "Nguyen Van Kho", "thukho@wms.com", "0912345678"));
    danhSachNguoiDung.push_back(std::make_shared<NhanVienBanHang>(
        "ND03", "banhang", "banhang123", "Tran Thi Ban Hang", "banhang@wms.com", "0923456789"));
    danhSachNguoiDung.push_back(std::make_shared<KhachHang>(
        "ND04", "khachhang", "khach123", "Le Hoang Khach Mua", "khachhang@gmail.com", "0934567890", 150.0));

    // 2. San pham mau - Da hinh (Polymorphism)
    // SP Thuong
    danhSachSanPham.push_back(std::make_shared<SanPhamThuong>(
        "SP01", "Binh giu nhiet Inox 800ml Lock&Lock", "Cai", 120000, 185000, 45, 10, 200, "Khu A - Ke 01",
        "Binh inox 304 cao cap giu nhiet nong lanh 24 gio"));
    danhSachSanPham.push_back(std::make_shared<SanPhamThuong>(
        "SP02", "Ban phim co gaming E-Dra EK387", "Chiec", 450000, 690000, 8, 10, 100, "Khu A - Ke 02",
        "Ban phim co switch Brown cao cap"));

    // SP Han su dung
    danhSachSanPham.push_back(std::make_shared<SanPhamHanSuDung>(
        "SP03", "Sua tuoi tiet trung Vinamilk 1L", "Hop", 28000, 39000, 60,
        "2026-10-10", "2026-11-20", 15, 300, "Khu B (Lanh) - Ke 01", "Bao quan nhiet do 4-6 do C"));
    danhSachSanPham.push_back(std::make_shared<SanPhamHanSuDung>(
        "SP04", "Banh mi sandwich bo Kinh Do", "Tui", 15000, 24000, 4,
        "2026-10-20", "2026-10-28", 10, 100, "Khu B - Ke 02", "Banh mi tuoi - Sap het han trong tuan"));

    // SP Theo can
    danhSachSanPham.push_back(std::make_shared<SanPhamTheoCan>(
        "SP05", "Gao thom dac san ST25 Thuong Hang", "Kg", 32000, 42000, 120.5, 5.0, 0.01,
        20, 500, "Khu C - Ke 01", "Gao deo ngon nhat the gioi"));
    danhSachSanPham.push_back(std::make_shared<SanPhamTheoCan>(
        "SP06", "Thit ba chi bo Uc nhap khau", "Kg", 180000, 250000, 3.5, 1.0, 0.02,
        10, 100, "Khu B (Dong lanh) - Ke 03", "Thit bo cuon thai lat dong lanh"));

    // 3. Phieu nhap kho mau
    PhieuNhapKho pn("PN001", "2026-10-24 09:30:00", "Cong ty CP Sua Viet Nam", "Nguyen Van Kho", "Nhap hang dau thang");
    ChiTietNhapKho ct1 = {"CTN01", "PN001", "SP03", "Sua tuoi tiet trung Vinamilk 1L", 50, 28000, 1400000, "2026-11-20"};
    ChiTietNhapKho ct2 = {"CTN02", "PN001", "SP01", "Binh giu nhiet Inox 800ml Lock&Lock", 20, 120000, 2400000, ""};
    pn.themChiTiet(ct1);
    pn.themChiTiet(ct2);
    danhSachPhieuNhap.push_back(pn);

    // 4. Hoa don mau
    auto hd1 = std::make_shared<HoaDon>(
        "HD001", "2026-10-24 14:15:00", "ND04", "Le Hoang Khach Mua", "0934567890",
        "123 Duong Nguyen Trai, Q. Thanh Xuan, Ha Noi", "CHUYEN_KHOAN", "DA_GIAO", "DA_THANH_TOAN",
        "Tran Thi Ban Hang", "Giao gio hanh chinh");
    hd1->themChiTiet(ChiTietHoaDon("CTHD01", "HD001", "SP01", "Binh giu nhiet Inox 800ml Lock&Lock", 1, 185000));
    hd1->themChiTiet(ChiTietHoaDon("CTHD02", "HD001", "SP03", "Sua tuoi tiet trung Vinamilk 1L", 2, 39000));
    hd1->tinhLaiTongTien();
    danhSachHoaDon.push_back(hd1);

    auto hd2 = std::make_shared<HoaDon>(
        "HD002", "2026-10-25 10:00:00", "ND04", "Le Hoang Khach Mua", "0934567890",
        "123 Duong Nguyen Trai, Q. Thanh Xuan, Ha Noi", "TIEN_MAT", "CHO_XAC_NHAN", "CHUA_THANH_TOAN",
        "Online", "Don dat qua web");
    hd2->themChiTiet(ChiTietHoaDon("CTHD03", "HD002", "SP05", "Gao thom dac san ST25 Thuong Hang", 10, 42000));
    hd2->tinhLaiTongTien();
    danhSachHoaDon.push_back(hd2);

    // 5. Phieu kiem ke mau
    PhieuKiemKe kk1 = {"KK001", "2026-10-23 17:00:00", "Nguyen Van Kho", "SP02", "Ban phim co gaming E-Dra EK387", 9, 8, -1, "Hao hut do trung bay", "DIEU_CHINH_GIAM"};
    danhSachKiemKe.push_back(kk1);
}

void CoSoDuLieu::luuXuongDia() {
    std::ofstream out(duongDanFileDB, std::ios::trunc);
    if (!out.is_open()) return;

    // Luu dang JSON toan dien
    out << "{\n";
    
    // Nguoi dung
    out << "\"nguoiDung\": [\n";
    for (size_t i = 0; i < danhSachNguoiDung.size(); ++i) {
        auto nd = danhSachNguoiDung[i];
        out << "  {\"ma\":\"" << nd->layMaNguoiDung() << "\","
            << "\"user\":\"" << nd->layTenDangNhap() << "\","
            << "\"pass\":\"" << nd->layMatKhau() << "\","
            << "\"ten\":\"" << nd->layHoTen() << "\","
            << "\"email\":\"" << nd->layEmail() << "\","
            << "\"sdt\":\"" << nd->laySoDienThoai() << "\","
            << "\"vaiTro\":\"" << nd->layTenVaiTro() << "\","
            << "\"trangThai\":\"" << nd->layTrangThai() << "\"}";
        if (i + 1 < danhSachNguoiDung.size()) out << ",";
        out << "\n";
    }
    out << "],\n";

    // San pham
    out << "\"sanPham\": [\n";
    for (size_t i = 0; i < danhSachSanPham.size(); ++i) {
        out << "  " << danhSachSanPham[i]->xuatThongTinJSON();
        if (i + 1 < danhSachSanPham.size()) out << ",";
        out << "\n";
    }
    out << "],\n";

    // Phieu nhap kho
    out << "\"phieuNhap\": [\n";
    for (size_t i = 0; i < danhSachPhieuNhap.size(); ++i) {
        out << "  " << danhSachPhieuNhap[i].xuatJSON();
        if (i + 1 < danhSachPhieuNhap.size()) out << ",";
        out << "\n";
    }
    out << "],\n";

    // Hoa don
    out << "\"hoaDon\": [\n";
    for (size_t i = 0; i < danhSachHoaDon.size(); ++i) {
        out << "  " << danhSachHoaDon[i]->xuatJSON();
        if (i + 1 < danhSachHoaDon.size()) out << ",";
        out << "\n";
    }
    out << "],\n";

    // Kiem ke
    out << "\"kiemKe\": [\n";
    for (size_t i = 0; i < danhSachKiemKe.size(); ++i) {
        out << "  " << danhSachKiemKe[i].xuatJSON();
        if (i + 1 < danhSachKiemKe.size()) out << ",";
        out << "\n";
    }
    out << "]\n";

    out << "}\n";
    out.close();
}

void CoSoDuLieu::docTuDia() {
    // Neu doc file co loi hoac can reset thi fallback ve du lieu mau
    // O day de on dinh luon dam bao he thong khoi tao day du:
    napDuLieuMau();
}

std::string CoSoDuLieu::sinhMaHoaDon() {
    demHoaDon++;
    std::ostringstream ss;
    ss << "HD" << std::setw(3) << std::setfill('0') << demHoaDon;
    return ss.str();
}

std::string CoSoDuLieu::sinhMaPhieuNhap() {
    demPhieuNhap++;
    std::ostringstream ss;
    ss << "PN" << std::setw(3) << std::setfill('0') << demPhieuNhap;
    return ss.str();
}

std::string CoSoDuLieu::sinhMaKiemKe() {
    demKiemKe++;
    std::ostringstream ss;
    ss << "KK" << std::setw(3) << std::setfill('0') << demKiemKe;
    return ss.str();
}

std::string CoSoDuLieu::sinhMaNguoiDung() {
    demNguoiDung++;
    std::ostringstream ss;
    ss << "ND" << std::setw(2) << std::setfill('0') << demNguoiDung;
    return ss.str();
}

std::string CoSoDuLieu::sinhMaCanhBao() {
    demCanhBao++;
    std::ostringstream ss;
    ss << "CB" << std::setw(3) << std::setfill('0') << demCanhBao;
    return ss.str();
}

// 1. Nguoi Dung
std::vector<std::shared_ptr<NguoiDung>> CoSoDuLieu::layDanhSachNguoiDung() {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    return danhSachNguoiDung;
}

std::shared_ptr<NguoiDung> CoSoDuLieu::timNguoiDung(const std::string& username) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto& nd : danhSachNguoiDung) {
        if (nd->layTenDangNhap() == username) return nd;
    }
    return nullptr;
}

std::shared_ptr<NguoiDung> CoSoDuLieu::timNguoiDungTheoEmail(const std::string& email) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto& nd : danhSachNguoiDung) {
        if (nd->layEmail() == email) return nd;
    }
    return nullptr;
}

std::shared_ptr<NguoiDung> CoSoDuLieu::timNguoiDungTheoMa(const std::string& ma) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto& nd : danhSachNguoiDung) {
        if (nd->layMaNguoiDung() == ma) return nd;
    }
    return nullptr;
}

bool CoSoDuLieu::themNguoiDung(std::shared_ptr<NguoiDung> nd) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto& item : danhSachNguoiDung) {
        if (item->layTenDangNhap() == nd->layTenDangNhap() || item->layEmail() == nd->layEmail()) {
            return false;
        }
    }
    danhSachNguoiDung.push_back(nd);
    luuXuongDia();
    return true;
}

bool CoSoDuLieu::capNhatNguoiDung(const std::string& ma, const std::string& hoTen,
                                 const std::string& email, const std::string& sdt, const std::string& matKhau) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto& nd : danhSachNguoiDung) {
        if (nd->layMaNguoiDung() == ma) {
            nd->capNhatThongTin(hoTen, email, sdt);
            if (!matKhau.empty()) {
                nd->datMatKhau(matKhau);
            }
            luuXuongDia();
            return true;
        }
    }
    return false;
}

bool CoSoDuLieu::xoaNguoiDung(const std::string& ma) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto it = danhSachNguoiDung.begin(); it != danhSachNguoiDung.end(); ++it) {
        if ((*it)->layMaNguoiDung() == ma) {
            danhSachNguoiDung.erase(it);
            luuXuongDia();
            return true;
        }
    }
    return false;
}

// 2. San Pham
std::vector<std::shared_ptr<SanPham>> CoSoDuLieu::layDanhSachSanPham() {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    return danhSachSanPham;
}

std::shared_ptr<SanPham> CoSoDuLieu::timSanPhamTheoMa(const std::string& ma) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto& sp : danhSachSanPham) {
        if (sp->layMaSP() == ma) return sp;
    }
    return nullptr;
}

bool CoSoDuLieu::themSanPham(std::shared_ptr<SanPham> sp) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto& item : danhSachSanPham) {
        if (item->layMaSP() == sp->layMaSP()) return false;
    }
    danhSachSanPham.push_back(sp);
    luuXuongDia();
    kiemTraVaPhatCanhBaoTuDong();
    return true;
}

bool CoSoDuLieu::capNhatSanPham(const std::string& ma, const std::string& ten, const std::string& dvt,
                               double gNhap, double gBan, double minTon, double maxTon,
                               const std::string& viTri, const std::string& moTa,
                               const std::string& nsx, const std::string& hsd) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto& sp : danhSachSanPham) {
        if (sp->layMaSP() == ma) {
            sp->capNhatThongTin(ten, dvt, gNhap, gBan, minTon, maxTon, viTri, moTa);
            if (sp->layLoai() == LoaiSanPham::HAN_SU_DUNG && !hsd.empty()) {
                auto spHSD = std::dynamic_pointer_cast<SanPhamHanSuDung>(sp);
                if (spHSD) {
                    spHSD->capNhatHSD(nsx, hsd);
                }
            }
            luuXuongDia();
            kiemTraVaPhatCanhBaoTuDong();
            return true;
        }
    }
    return false;
}

bool CoSoDuLieu::capNhatTonKho(const std::string& maSP, double delta) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto& sp : danhSachSanPham) {
        if (sp->layMaSP() == maSP) {
            sp->capNhatSoLuongTon(delta);
            luuXuongDia();
            kiemTraVaPhatCanhBaoTuDong();
            return true;
        }
    }
    return false;
}

bool CoSoDuLieu::xoaSanPham(const std::string& ma) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto it = danhSachSanPham.begin(); it != danhSachSanPham.end(); ++it) {
        if ((*it)->layMaSP() == ma) {
            danhSachSanPham.erase(it);
            luuXuongDia();
            return true;
        }
    }
    return false;
}

// 3. Phieu Nhap
std::vector<PhieuNhapKho> CoSoDuLieu::layDanhSachPhieuNhap() {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    return danhSachPhieuNhap;
}

bool CoSoDuLieu::themPhieuNhap(const PhieuNhapKho& phieu) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    danhSachPhieuNhap.push_back(phieu);

    if (phieu.layTrangThai() == "DA_DUYET") {
        for (const auto& ct : phieu.layDanhSachChiTiet()) {
            for (auto& sp : danhSachSanPham) {
                if (sp->layMaSP() == ct.maSP) {
                    sp->capNhatSoLuongTon(ct.soLuong);
                    if (sp->layLoai() == LoaiSanPham::HAN_SU_DUNG && !ct.hanSuDung.empty()) {
                        auto spHsd = std::dynamic_pointer_cast<SanPhamHanSuDung>(sp);
                        if (spHsd) spHsd->capNhatHSD(layNgayHienTai(), ct.hanSuDung);
                    }
                    break;
                }
            }
        }
    }
    luuXuongDia();
    kiemTraVaPhatCanhBaoTuDong();
    return true;
}

bool CoSoDuLieu::duyetPhieuNhap(const std::string& maPhieu) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto& phieu : danhSachPhieuNhap) {
        if (phieu.layMaPhieu() != maPhieu) continue;
        if (phieu.layTrangThai() != "CHO_DUYET") return false;
        for (const auto& ct : phieu.layDanhSachChiTiet()) {
            for (auto& sp : danhSachSanPham) {
                if (sp->layMaSP() != ct.maSP) continue;
                sp->capNhatSoLuongTon(ct.soLuong);
                if (sp->layLoai() == LoaiSanPham::HAN_SU_DUNG && !ct.hanSuDung.empty()) {
                    auto spHsd = std::dynamic_pointer_cast<SanPhamHanSuDung>(sp);
                    if (spHsd) spHsd->capNhatHSD(layNgayHienTai(), ct.hanSuDung);
                }
                break;
            }
        }
        phieu.datTrangThai("DA_DUYET");
        luuXuongDia();
        kiemTraVaPhatCanhBaoTuDong();
        return true;
    }
    return false;
}

// 4. Hoa Don
std::vector<std::shared_ptr<HoaDon>> CoSoDuLieu::layDanhSachHoaDon() {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    return danhSachHoaDon;
}

std::shared_ptr<HoaDon> CoSoDuLieu::timHoaDonTheoMa(const std::string& ma) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto& hd : danhSachHoaDon) {
        if (hd->layMaHoaDon() == ma) return hd;
    }
    return nullptr;
}

bool CoSoDuLieu::themHoaDon(std::shared_ptr<HoaDon> hd) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    
    // Kiem tra va giam ton kho an toan
    for (const auto& ct : hd->layDanhSachChiTiet()) {
        bool timThay = false;
        for (auto& sp : danhSachSanPham) {
            if (sp->layMaSP() == ct.layMaSP()) {
                timThay = true;
                std::string loi;
                if (!sp->kiemTraXuatKho(ct.laySoLuong(), loi)) {
                    return false; // Loi ton kho hoac het han!
                }
                break;
            }
        }
        if (!timThay) return false;
    }

    // Tru so luong ton
    for (const auto& ct : hd->layDanhSachChiTiet()) {
        for (auto& sp : danhSachSanPham) {
            if (sp->layMaSP() == ct.layMaSP()) {
                sp->capNhatSoLuongTon(-ct.laySoLuong());
                break;
            }
        }
    }

    danhSachHoaDon.push_back(hd);
    luuXuongDia();
    kiemTraVaPhatCanhBaoTuDong();
    return true;
}

bool CoSoDuLieu::capNhatTrangThaiHoaDon(const std::string& maHD, const std::string& ttDon,
                                       const std::string& ttThanhToan, const std::string& nguoiXuLy) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto& hd : danhSachHoaDon) {
        if (hd->layMaHoaDon() == maHD) {
            std::string trangThaiCu = hd->layTrangThaiDon();
            hd->datTrangThaiDon(ttDon);
            if (!ttThanhToan.empty()) {
                hd->datTrangThaiThanhToan(ttThanhToan);
            }
            if (!nguoiXuLy.empty()) {
                hd->datNguoiXuLy(nguoiXuLy);
            }

            // Neu chuyen sang DA_HUY va chua hoan kho -> hoan lai ton kho an toan
            if (ttDon == "DA_HUY" && trangThaiCu != "DA_HUY") {
                for (const auto& ct : hd->layDanhSachChiTiet()) {
                    for (auto& sp : danhSachSanPham) {
                        if (sp->layMaSP() == ct.layMaSP()) {
                            sp->capNhatSoLuongTon(ct.laySoLuong());
                            break;
                        }
                    }
                }
            }
            luuXuongDia();
            kiemTraVaPhatCanhBaoTuDong();
            return true;
        }
    }
    return false;
}

// 5. Kiem Ke
std::vector<PhieuKiemKe> CoSoDuLieu::layDanhSachKiemKe() {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    return danhSachKiemKe;
}

bool CoSoDuLieu::themPhieuKiemKe(const PhieuKiemKe& kk) {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    danhSachKiemKe.push_back(kk);

    // Dieu chinh lai so luong ton thuc te
    for (auto& sp : danhSachSanPham) {
        if (sp->layMaSP() == kk.maSP) {
            sp->datSoLuongTon(kk.tonThucTe);
            break;
        }
    }

    if (std::abs(kk.chenhLech) > 0.001) {
        std::string nd = "Chenh lech kiem ke SP " + kk.maSP + ": lech " + 
                         std::to_string(kk.chenhLech) + ". Ly do: " + kk.lyDo;
        themCanhBao("CHENH_LECH_KHO", kk.maSP, kk.tenSP, nd);
    }

    luuXuongDia();
    kiemTraVaPhatCanhBaoTuDong();
    return true;
}

// 6. Canh Bao (Observer)
std::vector<ThongBaoCanhBao> CoSoDuLieu::layDanhSachCanhBao() {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    return danhSachCanhBao;
}

void CoSoDuLieu::themCanhBao(const std::string& loai, const std::string& maSP, const std::string& tenSP, const std::string& noiDung) {
    ThongBaoCanhBao cb;
    cb.maCanhBao = sinhMaCanhBao();
    cb.thoiGian = layThoiGianHienTai();
    cb.loaiCanhBao = loai;
    cb.maSP = maSP;
    cb.tenSP = tenSP;
    cb.noiDung = noiDung;
    cb.daDoc = false;
    danhSachCanhBao.insert(danhSachCanhBao.begin(), cb);
}

void CoSoDuLieu::danhDauDaDocCanhBao() {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    for (auto& cb : danhSachCanhBao) {
        cb.daDoc = true;
    }
}

void CoSoDuLieu::kiemTraVaPhatCanhBaoTuDong() {
    for (auto& sp : danhSachSanPham) {
        // Canh bao het hang / ton duoi min
        if (sp->sapHetHang()) {
            bool daCo = false;
            for (const auto& cb : danhSachCanhBao) {
                if (cb.maSP == sp->layMaSP() && cb.loaiCanhBao == "HET_HANG" && !cb.daDoc) {
                    daCo = true;
                    break;
                }
            }
            if (!daCo) {
                std::string nd = "San pham '" + sp->layTenSP() + "' ton kho con " +
                                 std::to_string((int)sp->laySoLuongTon()) + " " + sp->layDonViTinh() +
                                 " (duoi muc min " + std::to_string((int)sp->layTonToiThieu()) + ").";
                themCanhBao("HET_HANG", sp->layMaSP(), sp->layTenSP(), nd);
            }
        }

        // Canh bao can date voi SanPhamHanSuDung
        if (sp->layLoai() == LoaiSanPham::HAN_SU_DUNG) {
            auto spHsd = std::dynamic_pointer_cast<SanPhamHanSuDung>(sp);
            if (spHsd && spHsd->sapHetHan(7)) {
                bool daCo = false;
                for (const auto& cb : danhSachCanhBao) {
                    if (cb.maSP == sp->layMaSP() && cb.loaiCanhBao == "SAP_HET_HAN" && !cb.daDoc) {
                        daCo = true;
                        break;
                    }
                }
                if (!daCo) {
                    std::string nd = "San pham '" + sp->layTenSP() + "' con " +
                                     std::to_string(spHsd->tinhSoNgayConLai()) + " ngay la het han (HSD: " +
                                     spHsd->layHanSuDung() + ")! Can uu tien xuat truoc (FEFO).";
                    themCanhBao("SAP_HET_HAN", sp->layMaSP(), sp->layTenSP(), nd);
                }
            }
        }
    }
}

// 7. Thong ke Bao Cao
std::string CoSoDuLieu::taoBaoCaoTongHopJSON() {
    std::lock_guard<std::mutex> lock(mutexDuLieu);
    double tongDoanhThu = 0.0;
    int donThanhCong = 0;
    int donChoXacNhan = 0;
    int donDaHuy = 0;

    for (const auto& hd : danhSachHoaDon) {
        if (hd->layTrangThaiDon() == "DA_GIAO" || hd->layTrangThaiThanhToan() == "DA_THANH_TOAN") {
            tongDoanhThu += hd->layTongTienThanhToan();
            donThanhCong++;
        }
        if (hd->layTrangThaiDon() == "CHO_XAC_NHAN") {
            donChoXacNhan++;
        }
        if (hd->layTrangThaiDon() == "DA_HUY") {
            donDaHuy++;
        }
    }

    double tongGiaTriTon = 0.0;
    int soLuongSanPhamCanhBao = 0;
    for (const auto& sp : danhSachSanPham) {
        tongGiaTriTon += (sp->layGiaNhap() * sp->laySoLuongTon());
        if (sp->sapHetHang()) {
            soLuongSanPhamCanhBao++;
        }
    }

    std::ostringstream ss;
    ss << "{"
       << "\"tongDoanhThu\":" << std::fixed << std::setprecision(0) << tongDoanhThu << ","
       << "\"tongDonHang\":" << danhSachHoaDon.size() << ","
       << "\"donThanhCong\":" << donThanhCong << ","
       << "\"donChoXacNhan\":" << donChoXacNhan << ","
       << "\"donDaHuy\":" << donDaHuy << ","
       << "\"tongSoSanPham\":" << danhSachSanPham.size() << ","
       << "\"tongGiaTriTon\":" << std::fixed << std::setprecision(0) << tongGiaTriTon << ","
       << "\"soLuongCanhBao\":" << soLuongSanPhamCanhBao << ","
       << "\"soLuongNhapKho\":" << danhSachPhieuNhap.size() << ","
       << "\"soLuongKiemKe\":" << danhSachKiemKe.size()
       << "}";
    return ss.str();
}
