// Lab401 Catalog JS for CyberFlipper
fetch('assets/lab401_catalog.json')
  .then(res => res.json())
  .then(data => {
    const list = document.getElementById('catalog-list');
    data.forEach(product => {
      const col = document.createElement('div');
      col.className = 'col-md-4';
      col.innerHTML = `
        <div class="glass-card p-4 h-100 d-flex flex-column justify-content-between">
          <h3 class="hud-label text-cyan mb-2">${product.title}</h3>
          ${product.image ? `<img src="${product.image}" alt="${product.title}" class="img-fluid mb-3">` : ''}
          <div class="mb-2 text-white-50">${product.description || ''}</div>
          <div class="mb-2"><span class="badge bg-cyan">${product.price || ''}</span></div>
          <a href="${product.url}" class="btn btn-cyber w-100 mt-auto" target="_blank">View & Buy on Lab401</a>
        </div>
      `;
      list.appendChild(col);
    });
  });
