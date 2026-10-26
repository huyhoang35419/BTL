// ========================================================
// CORE JAVASCRIPT: KET NOI REST API BACKEND C++
// ========================================================

const API_BASE = '';

function dinhDangTien(soTien) {
    if (!soTien) return '0 ₫';
    return Number(soTien).toLocaleString('vi-VN') + ' ₫';
}

function layGioHang() {
    const gh = localStorage.getItem('wms_cart');
    return gh ? JSON.parse(gh) : [];
}

function luuGioHang(gh) {
    localStorage.setItem('wms_cart', JSON.stringify(gh));
    capNhatSoLuongGioHangUI();
}

function themVaoGioHang(maSP, tenSP, giaBan, loaiSP) {
    let gh = layGioHang();
    let item = gh.find(x => x.maSP === maSP);
    if (item) {
        item.soLuong += 1;
    } else {
        gh.push({ maSP, tenSP, giaBan, loaiSP, soLuong: 1 });
    }
    luuGioHang(gh);
    alert('Đã thêm sản phẩm "' + tenSP + '" vào giỏ hàng!');
}

function capNhatSoLuongGioHangUI() {
    const badge = document.getElementById('cart-count-badge');
    if (badge) {
        const gh = layGioHang();
        const total = gh.reduce((sum, item) => sum + item.soLuong, 0);
        badge.innerText = total;
        badge.style.display = total > 0 ? 'inline-block' : 'none';
    }
}

// ========================================================
// GIOI THIEU THU THAP CANH BAO OBSERVER PATTERN
// ========================================================
async function taiCanhBaoObserver() {
    const user = Auth.layNguoiDungHienTai();
    if (user && user.vaiTro === 'KHACH_HANG') return;

    try {
        const res = await fetch('/api/kho/canh-bao');
        const data = await res.json();
        const banner = document.getElementById('observer-alert-banner');
        const countBadge = document.getElementById('alert-count-badge');
        
        if (data.thanhCong && data.danhSach && data.danhSach.length > 0) {
            const chuaDoc = data.danhSach.filter(x => !x.daDoc);
            if (countBadge) {
                countBadge.innerText = chuaDoc.length;
                countBadge.style.display = chuaDoc.length > 0 ? 'inline-block' : 'none';
            }

            if (banner && chuaDoc.length > 0) {
                const dauTien = chuaDoc[0];
                let alertClass = 'alert-box alert-warning';
                let icon = '⚠️ [CẢNH BÁO TỒN KHO]';
                if (dauTien.loaiCanhBao === 'SAP_HET_HAN') {
                    alertClass = 'alert-box alert-danger';
                    icon = '⏳ [CẢNH BÁO HẠN DÙNG - FEFO]';
                }

                banner.className = alertClass;
                banner.style.display = 'flex';
                banner.innerHTML = `
                    <div>
                        <strong>${icon} Observer Pattern:</strong> ${dauTien.noiDung}
                    </div>
                    <button class="btn btn-outline btn-sm" onclick="danhDauDaDocTatCaCanhBao()">Đã xem (${chuaDoc.length})</button>
                `;
            }
        }
    } catch (e) {
        console.error('Loi tai canh bao:', e);
    }
}

async function danhDauDaDocTatCaCanhBao() {
    try {
        await fetch('/api/kho/canh-bao/da-doc', { method: 'POST' });
        const banner = document.getElementById('observer-alert-banner');
        if (banner) banner.style.display = 'none';
        const countBadge = document.getElementById('alert-count-badge');
        if (countBadge) countBadge.style.display = 'none';
    } catch (e) {
        console.error(e);
    }
}

document.addEventListener('DOMContentLoaded', () => {
    capNhatSoLuongGioHangUI();
    taiCanhBaoObserver();
});

