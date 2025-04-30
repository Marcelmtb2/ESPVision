let fallbackInterval = null;
let sseConnected = false;
let lastUpdate = Date.now();
let refreshPaused = false;

async function reloadImages() {
    if (refreshPaused) return;

    // Update images and status visuals
    lastUpdate = Date.now();
    const ts = Date.now();
    ["qvga","mask","uxga"].forEach(id => {
      document.getElementById(id).src =
        `/static/current/${id}.jpg?_=${ts}`;
    });
  }

function setupSSE() {
    const evtSource = new EventSource("/events");
  
    evtSource.onmessage = function (event) {
      sseConnected = true;
      if (fallbackInterval !== null) {
        clearInterval(fallbackInterval);  // Stop polling
        fallbackInterval = null;
      }
      // refreshImages();
      reloadImages();
    };
  
    evtSource.onerror = function () {
      console.warn("SSE connection lost");
      evtSource.close();
      if (!sseConnected && fallbackInterval === null) {
        fallbackInterval = setInterval(reloadImages, 5000);
      }
    };
  }
  
  setupSSE();