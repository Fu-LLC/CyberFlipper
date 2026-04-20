/* =============================================
   CYBERFLIPPER — dashboard.js v2.0.0
   Neon Dusk Protocol — Unique animations
   ============================================= */

// ── BOOT SEQUENCE — Typewriter loading text ──
document.addEventListener('DOMContentLoaded', () => {
  const loadText = document.querySelector('.loading-text');
  if (loadText) {
    const lines = [
      'CYBERFLIPPER v1.3.0 BOOT',
      'Daily CVE feed: SYNCED',
      'SubGHz radios: 167 captures',
      'NFC stack: 50+ profiles',
      'BadUSB: CISA KEV ACTIVE',
      'System online'
    ];
    let i = 0;
    const typeNext = () => {
      if (i < lines.length) {
        loadText.textContent = lines[i];
        i++;
        setTimeout(typeNext, 280);
      }
    };
    typeNext();
  }

  // Dismiss after typewriter finishes (6 lines × 280ms = ~1680ms + buffer)
  setTimeout(() => {
    const ls = document.getElementById('loadingScreen');
    if (ls) ls.classList.add('hidden');
  }, 2200);

  // Fallback: force-remove after 5s no matter what
  setTimeout(() => {
    const ls = document.getElementById('loadingScreen');
    if (ls && !ls.classList.contains('hidden')) {
      ls.classList.add('hidden');
    }
  }, 5000);
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
        el.textContent = `${lat}\u00B0 N, ${Math.abs(lon).toFixed(4)}\u00B0 W`;
      },
      () => { el.textContent = '33.4792\u00B0 N, 112.0833\u00B0 W'; }
    );
  } else {
    el.textContent = '33.4792\u00B0 N, 112.0833\u00B0 W';
  }
})();

// ── COUNTER — Slot-machine digit roll ──
function animateCounter(el, target, duration = 2000) {
  let start = null;
  const step = ts => {
    if (!start) start = ts;
    const progress = Math.min((ts - start) / duration, 1);
    // Elastic ease-out for mechanical feel
    const ease = progress === 1 ? 1 : 1 - Math.pow(2, -10 * progress);
    el.textContent = Math.round(ease * target);
    if (progress < 1) requestAnimationFrame(step);
  };
  requestAnimationFrame(step);
}

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

// ── SIGNAL WAVE — Oscilloscope style with phosphor trail ──
(function signalWave() {
  const canvas = document.getElementById('signalWave');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  canvas.width = canvas.offsetWidth || 300;
  canvas.height = 60;

  let t = 0;
  let prevFrame = null;

  function draw() {
    const w = canvas.width, h = canvas.height;

    // Phosphor decay trail (don't fully clear)
    ctx.fillStyle = 'rgba(6,6,8,0.25)';
    ctx.fillRect(0, 0, w, h);

    // Center line
    ctx.strokeStyle = 'rgba(0,255,204,0.06)';
    ctx.lineWidth = 0.5;
    ctx.beginPath();
    ctx.moveTo(0, h / 2);
    ctx.lineTo(w, h / 2);
    ctx.stroke();

    // Main trace — sawtooth + sine composite
    ctx.beginPath();
    ctx.strokeStyle = 'rgba(0,255,204,0.85)';
    ctx.lineWidth = 1.5;
    ctx.shadowColor = 'rgba(0,255,204,0.5)';
    ctx.shadowBlur = 6;
    for (let x = 0; x <= w; x++) {
      const saw = ((x * 0.02 + t) % 1) * 2 - 1;
      const sin1 = Math.sin((x * 0.05) + t * 2.5) * 10;
      const sin2 = Math.sin((x * 0.12) + t * 1.1) * 4;
      const noise = Math.sin(x * 0.8 + t * 8) * 1.5;
      const y = h / 2 + sin1 + sin2 + saw * 3 + noise;
      x === 0 ? ctx.moveTo(x, y) : ctx.lineTo(x, y);
    }
    ctx.stroke();
    ctx.shadowBlur = 0;

    // Secondary trace — pink frequency
    ctx.beginPath();
    ctx.strokeStyle = 'rgba(255,0,110,0.3)';
    ctx.lineWidth = 1;
    for (let x = 0; x <= w; x++) {
      const y = h / 2 + Math.sin((x * 0.08) + t * 3.2 + 2) * 6 + Math.cos((x * 0.03) + t) * 4;
      x === 0 ? ctx.moveTo(x, y) : ctx.lineTo(x, y);
    }
    ctx.stroke();

    t += 0.03;
    requestAnimationFrame(draw);
  }
  draw();

  window.addEventListener('resize', () => { canvas.width = canvas.offsetWidth; });
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
