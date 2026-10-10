/**
 * ==============================================================================
 * Browser History Management Using Stacks - Frontend Controller
 * Connected to C Engine Backend via Lightweight Local REST Bridge
 * ==============================================================================
 */

// API Configuration
const API_BASE = window.location.origin.includes('http') ? window.location.origin : 'http://127.0.0.1:8000';

// Local UI state mirror
let state = {
    backStack: [],
    forwardStack: [],
    currentPage: null,
    hasCurrentPage: false,
    isConnected: false
};

// DOM Elements
const elements = {
    // Navigation & Forms
    urlForm: document.getElementById('urlForm'),
    urlInput: document.getElementById('urlInput'),
    btnVisit: document.getElementById('btnVisit'),
    btnBack: document.getElementById('btnBack'),
    btnForward: document.getElementById('btnForward'),
    btnClear: document.getElementById('btnClear'),
    presetChips: document.querySelectorAll('.preset-chip'),

    // Stack Containers & Counters
    backStackContainer: document.getElementById('backStackContainer'),
    forwardStackContainer: document.getElementById('forwardStackContainer'),
    backStackCount: document.getElementById('backStackCount'),
    forwardStackCount: document.getElementById('forwardStackCount'),
    statBackCount: document.getElementById('statBackCount'),
    statForwardCount: document.getElementById('statForwardCount'),

    // Viewport & Active Page
    tabTitle: document.getElementById('tabTitle'),
    currentUrlText: document.getElementById('currentUrlText'),
    pageStatusPill: document.getElementById('pageStatusPill'),
    pageStatusText: document.getElementById('pageStatusText'),
    lastActionBadge: document.getElementById('lastActionBadge'),
    previewTitle: document.getElementById('previewTitle'),
    previewDescription: document.getElementById('previewDescription'),

    // Logging & Console
    consoleMessages: document.getElementById('consoleMessages'),
    btnClearLog: document.getElementById('btnClearLog')
};

/**
 * Format current timestamp for log messages (HH:MM:SS)
 */
function getTimestamp() {
    const now = new Date();
    return now.toTimeString().split(' ')[0];
}

/**
 * Append an entry to the status message log
 * @param {string} tag - Tag e.g. 'VISIT', 'BACK', 'FORWARD', 'CLEAR', 'INFO', 'ERROR'
 * @param {string} text - Message description
 * @param {string} type - 'info' | 'success' | 'warning' | 'error'
 */
function appendLog(tag, text, type = 'info') {
    const entry = document.createElement('div');
    entry.className = `log-entry log-${type}`;
    entry.innerHTML = `
        <span class="log-time">${getTimestamp()}</span>
        <span class="log-tag">${tag}</span>
        <span class="log-text">${text}</span>
    `;
    elements.consoleMessages.appendChild(entry);
    elements.consoleMessages.scrollTop = elements.consoleMessages.scrollHeight;
}

/**
 * Render a single stack's visual representation into its container
 * Shows items from TOP down to BOTTOM (LIFO order)
 */
function renderStackContainer(container, stackArray, isBackStack) {
    container.innerHTML = '';

    if (!stackArray || stackArray.length === 0) {
        const emptyDiv = document.createElement('div');
        emptyDiv.className = 'empty-state';
        emptyDiv.innerHTML = `
            <div class="empty-icon">📂</div>
            <p class="empty-text">${isBackStack ? 'Back stack is empty' : 'Forward stack is empty'}</p>
            <span class="empty-hint">${isBackStack ? 'Visit websites to push history onto this stack' : 'Navigating back will push pages here'}</span>
        `;
        container.appendChild(emptyDiv);
        return;
    }

    // Stack is rendered from top to bottom (LIFO visual)
    // The last element of stackArray represents the top of the stack (stack.top)
    for (let i = stackArray.length - 1; i >= 0; i--) {
        const itemUrl = stackArray[i];
        const isTop = (i === stackArray.length - 1);

        const itemEl = document.createElement('div');
        itemEl.className = `stack-item ${isTop ? 'is-top' : ''}`;
        itemEl.innerHTML = `
            <div class="item-left">
                <span class="item-index">[${i}]</span>
                <span class="item-url" title="${itemUrl}">${itemUrl}</span>
            </div>
            ${isTop ? '<span class="item-badge-top">TOP</span>' : ''}
        `;
        container.appendChild(itemEl);
    }
}

/**
 * Updates all UI elements based on the state received from the C backend
 */
function renderUI() {
    const backList = state.backStack || [];
    const forwardList = state.forwardStack || [];

    // 1. Update Back & Forward Buttons disabled states
    elements.btnBack.disabled = (backList.length === 0);
    elements.btnForward.disabled = (forwardList.length === 0);

    // 2. Update Stack Counters
    const backLen = backList.length;
    const fwdLen = forwardList.length;

    elements.backStackCount.textContent = `${backLen} ${backLen === 1 ? 'URL' : 'URLs'}`;
    elements.forwardStackCount.textContent = `${fwdLen} ${fwdLen === 1 ? 'URL' : 'URLs'}`;
    elements.statBackCount.textContent = backLen;
    elements.statForwardCount.textContent = fwdLen;

    // 3. Render Stack Cards
    renderStackContainer(elements.backStackContainer, backList, true);
    renderStackContainer(elements.forwardStackContainer, forwardList, false);

    // 4. Update Current Page Display
    if (state.hasCurrentPage && state.currentPage) {
        elements.tabTitle.textContent = state.currentPage;
        elements.currentUrlText.textContent = state.currentPage;
        elements.previewTitle.textContent = state.currentPage;
        elements.previewDescription.textContent = `Active page executing in C Stack Engine memory.`;
        elements.pageStatusPill.className = 'status-pill active';
        elements.pageStatusText.textContent = 'Active Page Loaded';
    } else {
        elements.tabTitle.textContent = 'No Page Loaded';
        elements.currentUrlText.textContent = 'No page opened yet';
        elements.previewTitle.textContent = 'Browser Ready';
        elements.previewDescription.textContent = 'Enter a website URL above or click a quick link to start navigation.';
        elements.pageStatusPill.className = 'status-pill';
        elements.pageStatusText.textContent = 'Idle';
    }
}

/**
 * Universal helper for calling backend API endpoints
 */
async function callApi(endpoint, method = 'GET', bodyData = null) {
    try {
        const options = {
            method,
            headers: {
                'Content-Type': 'application/json'
            }
        };
        if (bodyData) {
            options.body = JSON.stringify(bodyData);
        }

        const res = await fetch(`${API_BASE}${endpoint}`, options);
        const data = await res.json();

        if (!res.ok) {
            throw new Error(data.error || `HTTP error ${res.status}`);
        }

        // Apply updated state from C backend
        state.backStack = data.backStack || [];
        state.forwardStack = data.forwardStack || [];
        state.currentPage = data.currentPage || null;
        state.hasCurrentPage = Boolean(data.hasCurrentPage);
        state.isConnected = true;

        renderUI();
        return data;
    } catch (err) {
        console.error(`API Error on ${endpoint}:`, err);
        appendLog('ERROR', `${err.message}`, 'error');
        state.isConnected = false;
        return null;
    }
}

/**
 * Fetch initial state from the C backend on boot
 */
async function fetchInitialState() {
    appendLog('INIT', 'Connecting to C DSA Backend at ' + API_BASE + '...', 'info');
    const data = await callApi('/api/state');
    if (data) {
        appendLog('READY', 'Connected to C Backend Engine. Both stacks ready.', 'success');
        elements.lastActionBadge.textContent = 'Connected';
    } else {
        appendLog('WARN', 'Backend not reachable yet. Start server via "python server.py".', 'warning');
        elements.lastActionBadge.textContent = 'Offline';
    }
}

/**
 * CORE OPERATION 1: Visit New Page
 */
async function handleVisit(url) {
    const cleanUrl = url.trim();
    if (!cleanUrl) {
        appendLog('ERROR', 'Cannot visit an empty URL.', 'error');
        return;
    }

    elements.btnVisit.disabled = true;
    const data = await callApi('/api/visit', 'POST', { url: cleanUrl });
    elements.btnVisit.disabled = false;

    if (data) {
        elements.lastActionBadge.textContent = 'Visited Page';
        appendLog('VISIT', `C Engine: Successfully navigated to ${cleanUrl}`, 'success');
        appendLog('LIFO', `Previous page pushed to backStack, forwardStack cleared`, 'info');
    }
}

/**
 * CORE OPERATION 2: Go Back
 */
async function handleGoBack() {
    elements.btnBack.disabled = true;
    const data = await callApi('/api/back', 'POST');
    elements.btnBack.disabled = false;

    if (data) {
        elements.lastActionBadge.textContent = 'Went Back';
        appendLog('BACK', `C Engine: Went back to ${data.currentPage}`, 'success');
        appendLog('LIFO', `Popped top from backStack, pushed former page to forwardStack`, 'info');
    }
}

/**
 * CORE OPERATION 3: Go Forward
 */
async function handleGoForward() {
    elements.btnForward.disabled = true;
    const data = await callApi('/api/forward', 'POST');
    elements.btnForward.disabled = false;

    if (data) {
        elements.lastActionBadge.textContent = 'Went Forward';
        appendLog('FORWARD', `C Engine: Went forward to ${data.currentPage}`, 'success');
        appendLog('LIFO', `Popped top from forwardStack, pushed former page to backStack`, 'info');
    }
}

/**
 * CORE OPERATION 4: Clear History
 */
async function handleClearHistory() {
    elements.btnClear.disabled = true;
    const data = await callApi('/api/clear', 'POST');
    elements.btnClear.disabled = false;

    if (data) {
        elements.lastActionBadge.textContent = 'History Cleared';
        appendLog('CLEAR', 'C Engine: Both Back & Forward stacks reset to top = -1', 'warning');
    }
}

// Event Listeners
elements.urlForm.addEventListener('submit', (e) => {
    e.preventDefault();
    const url = elements.urlInput.value;
    handleVisit(url);
    elements.urlInput.value = '';
});

elements.btnBack.addEventListener('click', handleGoBack);
elements.btnForward.addEventListener('click', handleGoForward);
elements.btnClear.addEventListener('click', handleClearHistory);

// Preset quick links
elements.presetChips.forEach(chip => {
    chip.addEventListener('click', () => {
        const url = chip.getAttribute('data-url');
        handleVisit(url);
    });
});

// Clear console log
elements.btnClearLog.addEventListener('click', () => {
    elements.consoleMessages.innerHTML = '';
    appendLog('LOG', 'Console cleared.', 'info');
});

// Boot: Fetch current state from C backend
fetchInitialState();
