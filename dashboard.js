document.addEventListener('DOMContentLoaded', () => {
    // Hide loading screen after 2 seconds
    setTimeout(() => {
        const loading = document.getElementById('loadingScreen');
        loading.style.opacity = '0';
        setTimeout(() => loading.style.display = 'none', 1000);
    }, 2000);

    // Initial Uptime Clock
    let seconds = 0;
    const uptimeEl = document.getElementById('uptime-clock');
    setInterval(() => {
        seconds++;
        const h = Math.floor(seconds / 3600).toString().padStart(2, '0');
        const m = Math.floor((seconds % 3600) / 60).toString().padStart(2, '0');
        const s = (seconds % 60).toString().padStart(2, '0');
        uptimeEl.textContent = `${h}:${m}:${s}`;
    }, 1000);

    // Simulated Geo Location
    document.getElementById('geo-location').textContent = '33.419050 111.934573';

    // Counter Animation
    const counters = document.querySelectorAll('.counter, .value');
    counters.forEach(counter => {
        const target = +counter.getAttribute('data-val') || +counter.textContent;
        if (!target) return;
        
        let count = 0;
        const increment = target / 100;

        const updateCount = () => {
            if (count < target) {
                count += increment;
                counter.innerText = Math.ceil(count).toLocaleString();
                setTimeout(updateCount, 20);
            } else {
                counter.innerText = target.toLocaleString();
            }
        };

        if (counter.classList.contains('value')) {
             updateCount(); // Run immediately for hero stats
        } else {
            // Wait for intersection for main stats
            const observer = new IntersectionObserver(entries => {
                if (entries[0].isIntersecting) {
                    updateCount();
                    observer.unobserve(counter);
                }
            });
            observer.observe(counter);
        }
    });

    // Signal Wave Animation
    const canvas = document.getElementById('signalWave');
    const ctx = canvas.getContext('2d');
    let offset = 0;

    const draw = () => {
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        ctx.strokeStyle = '#00f3ff';
        ctx.lineWidth = 2;
        ctx.beginPath();
        
        for (let x = 0; x < canvas.width; x++) {
            const y = canvas.height / 2 + Math.sin(x * 0.05 + offset) * 20 + Math.sin(x * 0.02 + offset) * 10;
            if (x === 0) ctx.moveTo(x, y);
            else ctx.lineTo(x, y);
        }
        
        ctx.stroke();
        offset += 0.1;
        requestAnimationFrame(draw);
    };

    // Resize canvas
    const resize = () => {
        canvas.width = canvas.parentElement.clientWidth;
        canvas.height = 100;
    };
    window.addEventListener('resize', resize);
    resize();
    draw();

    // typing effect for terminal
    const terminalBody = document.querySelector('.terminal-body code');
    const content = terminalBody.innerHTML;
    terminalBody.innerHTML = '';
    let i = 0;
    const type = () => {
        if (i < content.length) {
            terminalBody.innerHTML = content.substring(0, i + 1);
            i++;
            setTimeout(type, 10);
        }
    };
    
    // Intersection observer for terminal
    const termObserver = new IntersectionObserver(entries => {
        if (entries[0].isIntersecting) {
            type();
            termObserver.unobserve(terminalBody);
        }
    });
    termObserver.observe(terminalBody);
    // Matrix Animation
    const mCanvas = document.getElementById('matrix-canvas');
    const mCtx = mCanvas.getContext('2d');
    
    mCanvas.width = window.innerWidth;
    mCanvas.height = window.innerHeight;
    
    const characters = "0123456789ABCDEF<>[]{}*&$#@!+-=";
    const fontSize = 14;
    const columns = mCanvas.width / fontSize;
    const drops = [];
    
    for (let x = 0; x < columns; x++) drops[x] = 1;

    const drawMatrix = () => {
        mCtx.fillStyle = "rgba(0, 0, 0, 0.05)";
        mCtx.fillRect(0, 0, mCanvas.width, mCanvas.height);
        
        mCtx.fillStyle = "#00f3ff";
        mCtx.font = fontSize + "px 'Courier New'";
        
        for (let i = 0; i < drops.length; i++) {
            const text = characters.charAt(Math.floor(Math.random() * characters.length));
            mCtx.fillText(text, i * fontSize, drops[i] * fontSize);
            
            if (drops[i] * fontSize > mCanvas.height && Math.random() > 0.975) {
                drops[i] = 0;
            }
            drops[i]++;
        }
    };
    
    setInterval(drawMatrix, 50);

    window.addEventListener('resize', () => {
        mCanvas.width = window.innerWidth;
        mCanvas.height = window.innerHeight;
    });

    // FFT Signal Simulator
    const fftCanvas = document.getElementById('fftCanvas');
    if (fftCanvas) {
        const fftCtx = fftCanvas.getContext('2d');
        let width = fftCanvas.width = fftCanvas.offsetWidth;
        let height = fftCanvas.height = fftCanvas.offsetHeight;
        
        const drawFFT = () => {
            fftCtx.fillStyle = '#000';
            fftCtx.fillRect(0, 0, width, height);
            
            fftCtx.strokeStyle = '#00f3ff';
            fftCtx.lineWidth = 2;
            fftCtx.beginPath();
            
            fftCtx.moveTo(0, height * 0.8);
            for (let i = 0; i < width; i += 5) {
                // Generate 'spiky' signal noise
                let y = height * 0.8 - Math.random() * 20;
                // Add fixed peaks for carriers
                if (i > width * 0.3 && i < width * 0.35) y -= Math.random() * 100;
                if (i > width * 0.7 && i < width * 0.75) y -= Math.random() * 60;
                
                fftCtx.lineTo(i, y);
            }
            fftCtx.stroke();
            
            // Add gradient fill
            fftCtx.lineTo(width, height);
            fftCtx.lineTo(0, height);
            fftCtx.fillStyle = 'rgba(0, 243, 255, 0.1)';
            fftCtx.fill();
            
            requestAnimationFrame(drawFFT);
        };
        drawFFT();
    }

    // Matrix Rain Initialization
    function initMatrixRain() {
        const matrix = document.getElementById('matrix-rain');
        if (!matrix) return;
        const chars = '01F-SERIES_CYBERFLIPPER_FLLC_PERSONFU_PROTOCOL_DECRYPTED';
        for (let i = 0; i < 40; i++) {
            const col = document.createElement('div');
            col.style.cssText = `
                position: absolute;
                left: ${i * 2.5}%;
                top: ${Math.random() * -100}%;
                font-family: 'Courier New', monospace;
                font-size: 14px;
                color: var(--cyber-cyan);
                writing-mode: vertical-rl;
                text-orientation: upright;
                opacity: 0.1;
                animation: dash-fall ${10 + Math.random() * 15}s linear infinite;
                animation-delay: ${Math.random() * 5}s;
            `;
            let text = '';
            for (let j = 0; j < 40; j++) {
                text += chars[Math.floor(Math.random() * chars.length)];
            }
            col.textContent = text;
            matrix.appendChild(col);
        }
    }
    initMatrixRain();

    // Sound Wave Simulation Updates
    function initSoundWaves() {
        const terminal = document.getElementById('liveTerminal');
        if (terminal) {
            const messages = [
                "[OK] Neural Mesh Synchronized",
                "[INFO] CC1101 active @ 433.92MHz",
                "[RSSI] -84.2 dBm | Locked",
                "[MAC] 4F:33:DE:AD:BE:EF -> Ingested",
                "[CMD] Sub-GHz Sniffer Active",
                "[SYS] F-SERIES v1.1.0 Operational",
                "[WARN] Unknown Protocol detected @ 868MHz",
                "[INFO] WEP/WPA handshake captured",
                "[GPS] 33.4255 N, 111.9400 W [F-SERIES_NODE]"
            ];
            
            setInterval(() => {
                const line = document.createElement('div');
                line.innerHTML = `> ${messages[Math.floor(Math.random() * messages.length)]} <span class="dash-sound-wave"><span class="dash-sound-bar"></span><span class="dash-sound-bar"></span><span class="dash-sound-bar"></span></span>`;
                terminal.appendChild(line);
                if (terminal.childNodes.length > 8) {
                    terminal.removeChild(terminal.firstChild);
                }
            }, 1500);
        }
    }
    initSoundWaves();

    // Simulated Live Telemetry Fluctuations
    setInterval(() => {
        const modEl = document.querySelectorAll('.tool-item .text-cyan')[0]?.nextSibling;
        if (modEl) {
            const mods = ['2-FSK', 'GFSK', 'MSK', 'OOK', 'ASK'];
            modEl.textContent = ' ' + mods[Math.floor(Math.random() * mods.length)];
        }
        
        const freqEl = document.querySelectorAll('.tool-item .text-cyan')[1]?.nextSibling;
        if (freqEl) {
            const drift = (Math.random() * 0.05 - 0.025).toFixed(4);
            freqEl.textContent = ' ' + (433.9200 + parseFloat(drift)).toFixed(4) + ' MHz';
        }
    }, 3000);
    
        // Ensure Lab401 Catalog link is present in navigation if dynamically generated
        if (typeof document !== 'undefined') {
            const nav = document.querySelector('nav.p-4');
            if (nav && !nav.innerHTML.includes('lab401.html')) {
                const lab401Link = document.createElement('a');
                lab401Link.href = '/CyberFlipper/lab401.html';
                lab401Link.className = 'nav-anchor';
                lab401Link.textContent = 'LAB401 CATALOG';
                nav.appendChild(lab401Link);
            }
        }
});
