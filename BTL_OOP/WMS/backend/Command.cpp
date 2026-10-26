#ifndef COMMAND_IMPL_H
#define COMMAND_IMPL_H

#include "Command.h"
#include "CoSoDuLieu.h"

// Lenh Huy Don Hang (Command Pattern)
class LenhHuyHoaDon : public ILenh {
private:
    std::string maHoaDon;
    std::string trangThaiCu;
    std::string trangThaiThanhToanCu;
    bool daTungHoanKho;
    std::shared_ptr<CoSoDuLieu> csdl;

public:
    LenhHuyHoaDon(const std::string& maHD, std::shared_ptr<CoSoDuLieu> db)
        : maHoaDon(maHD), daTungHoanKho(false), csdl(db) {}

    std::string layTenLenh() const override { return "HuyHoaDon"; }
    std::string layMaDoiTuong() const override { return maHoaDon; }

    bool thucHien(std::string& thongBaoLoi) override {
        auto hd = csdl->timHoaDonTheoMa(maHoaDon);
        if (!hd) {
            thongBaoLoi = "Khong tim thay hoa don " + maHoaDon;
            return false;
        }

        if (!hd->coTheHuy()) {
            thongBaoLoi = "Don hang da hoan tat hoac da bi huy truoc do!";
            return false;
        }

        trangThaiCu = hd->layTrangThaiDon();
        trangThaiThanhToanCu = hd->layTrangThaiThanhToan();

        // Kiem tra don hang da xuat kho hay chua de hoan ton kho an toan
        // Neu don chua hoan kho, thi moi hoan kho khi huy!
        bool thanhCong = csdl->capNhatTrangThaiHoaDon(maHoaDon, "DA_HUY", 
                               (trangThaiThanhToanCu == "DA_THANH_TOAN" ? "DA_HOAN_TIEN" : "CHUA_THANH_TOAN"),
                               "System-Command");
        if (thanhCong) {
            daTungHoanKho = true;
            return true;
        }
        thongBaoLoi = "Khong the cap nhat trang thai hoa don!";
        return false;
    }

    bool hoanTac(std::string& thongBaoLoi) override {
        auto hd = csdl->timHoaDonTheoMa(maHoaDon);
        if (!hd) {
            thongBaoLoi = "Khong tim thay hoa don de hoan tac!";
            return false;
        }

        // Hoan tac lai trang thai cu va tru lai ton kho neu truoc do da hoan kho
        if (daTungHoanKho) {
            for (const auto& ct : hd->layDanhSachChiTiet()) {
                auto sp = csdl->timSanPhamTheoMa(ct.layMaSP());
                if (sp) {
                    if (sp->laySoLuongTon() < ct.laySoLuong()) {
                        thongBaoLoi = "Ton kho khong du de phuc hoi don hang!";
                        return false;
                    }
                }
            }
            for (const auto& ct : hd->layDanhSachChiTiet()) {
                csdl->capNhatTonKho(ct.layMaSP(), -ct.laySoLuong());
            }
        }

        hd->datTrangThaiDon(trangThaiCu);
        hd->datTrangThaiThanhToan(trangThaiThanhToanCu);
        csdl->luuXuongDia();
        return true;
    }
};

#endif // COMMAND_IMPL_H

