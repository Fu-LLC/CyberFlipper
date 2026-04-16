/* =============================================
   CYBERFLIPPER — dashboard.js v1.2.0
   Animations, counters, signal wave, HUD
   ============================================= */

// ── LOADING SCREEN ──
window.addEventListener('load', () => {
  setTimeout(() => {
    const ls = document.getElementById('loadingScreen');
    if (ls) ls.classList.add('hidden');
  }, 2000);
});

// ── UPTIME CLOCK ──
(function uptimeClock() {
  const el = document.getElementById('uptime-clock');
  if (!el) return;
  const start = Date.now();
  setInterval(() => {
    const s = Math.floor((Date.now() - start) / 1000);
    const h = String(Math.floor(s / 3600)).padStart(2, '0');
    const m = String(Math.floor((s % 3600) / 60)).padStart(2, '0');
    const ss = String(s % 60).padStart(2, '0');
    el.textContent = `${h}:${m}:${ss}`;
  }, 1000);
})();

// ── GEOLOCATION ──
(function geoLocation() {
  const el = document.getElementById('geo-location');
  if (!el) return;
  if (navigator.geolocation) {
    navigator.geolocation.getCurrentPosition(
      pos => {
        const lat = pos.coords.latitude.toFixed(4);
        const lon = pos.coords.longitude.toFixed(4);
        el.textContent = `${lat}° N, ${Math.abs(lon)}° W`;
      },
      () => { el.textContent = '33.4792° N, 112.0833° W'; }
    );
  } else {
    el.textContent = '33.4792° N, 112.0833° W';
  }
})();

// ── COUNTER ANIMATION ──
function animateCounter(el, target, duration = 1800) {
  let start = null;
  const step = ts => {
    if (!start) start = ts;
    const progress = Math.min((ts - start) / duration, 1);
    const ease = 1 - Math.pow(1 - progress, 3);
    el.textContent = Math.round(ease * target);
    if (progress < 1) requestAnimationFrame(step);
  };
  requestAnimationFrame(step);
}

// Start counters when visible
const counters = document.querySelectorAll('.counter');
if (counters.length) {
  const obs = new IntersectionObserver(entries => {
    entries.forEach(e => {
      if (e.isIntersecting) {
        const el = e.target;
        const target = parseInt(el.textContent.replace(/\D/g, '')) || parseInt(el.dataset.val) || 0;
        el.dataset.val = target;
        animateCounter(el, target);
        obs.unobserve(el);
      }
    });
  }, { threshold: 0.3 });
  counters.forEach(c => obs.observe(c));
}

// Holographic stat counters (data-val attribute)
document.querySelectorAll('.stat-item .value[data-val]').forEach(el => {
  const target = parseInt(el.dataset.val);
  const obs = new IntersectionObserver(entries => {
    if (entries[0].isIntersecting) {
      animateCounter(el, target);
      obs.disconnect();
    }
  }, { threshold: 0.3 });
  obs.observe(el);
});

// ── SIGNAL WAVE CANVAS ──
(function signalWave() {
  const canvas = document.getElementById('signalWave');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  canvas.width = canvas.offsetWidth || 300;
  canvas.height = 60;

  let t = 0;
  function draw() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    const w = canvas.width, h = canvas.height;

    // Wave 1 — cyan
    ctx.beginPath();
    ctx.strokeStyle = 'rgba(0,255,204,0.7)';
    ctx.lineWidth = 1.5;
    for (let x = 0; x <= w; x++) {
      const y = h / 2 + Math.sin((x * 0.04) + t) * 12 + Math.sin((x * 0.02) + t * 0.7) * 6;
      x === 0 ? ctx.moveTo(x, y) : ctx.lineTo(x, y);
    }
    ctx.stroke();

    // Wave 2 — purple
    ctx.beginPath();
    ctx.strokeStyle = 'rgba(124,58,237,0.45)';
    ctx.lineWidth = 1;
    for (let x = 0; x <= w; x++) {
      const y = h / 2 + Math.sin((x * 0.06) + t * 1.3 + 1) * 8;
      x === 0 ? ctx.moveTo(x, y) : ctx.lineTo(x, y);
    }
    ctx.stroke();

    t += 0.04;
    requestAnimationFrame(draw);
  }
  draw();

  window.addEventListener('resize', () => {
    canvas.width = canvas.offsetWidth;
  });
})();

// ── SMOOTH SCROLL ──
document.querySelectorAll('a[href^="#"]').forEach(a => {
  a.addEventListener('click', e => {
    const target = document.querySelector(a.getAttribute('href'));
    if (target) {
      e.preventDefault();
      target.scrollIntoView({ behavior: 'smooth', block: 'start' });
    }
  });
});

// ── ACTIVE NAV ON SCROLL ──
window.addEventListener('scroll', () => {
  const sections = document.querySelectorAll('section[id]');
  let current = '';
  sections.forEach(s => {
    if (scrollY >= s.offsetTop - 120) current = s.id;
  });
  document.querySelectorAll('.cyber-nav .nav-link').forEach(link => {
    link.classList.toggle('active', link.getAttribute('href') === '#' + current);
  });
}, { passive: true });
