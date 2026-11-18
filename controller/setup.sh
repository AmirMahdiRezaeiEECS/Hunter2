#!/bin/bash
set -e
PROJECT="robot-controller"
echo "Creating Robot Controller PWA..."
# Create directories
mkdir -p "$PROJECT/icons"
# Copy existing controller
if [ ! -f "robot_controller.html" ]; then
    echo "Error: robot_controller.html not found!"
    exit 1
fi
cp robot_controller.html "$PROJECT/index.html"
# Create manifest
cat > "$PROJECT/manifest.json" <<'EOF'
{
  "id": "/",
  "name": "Robot Controller",
  "short_name": "Robot",
  "description": "Wi-Fi robot car controller",
  "start_url": "/",
  "scope": "/",
  "display": "standalone",
  "orientation": "landscape",
  "background_color": "#030807",
  "theme_color": "#030807",
  "icons": [
    {
      "src": "/icons/icon-192.png",
      "sizes": "192x192",
      "type": "image/png"
    },
    {
      "src": "/icons/icon-512.png",
      "sizes": "512x512",
      "type": "image/png"
    }
  ]
}
EOF
# Create service worker
cat > "$PROJECT/service-worker.js" <<'EOF'
const CACHE_NAME = "robot-controller-v1";
const FILES = [
  "/",
  "/index.html",
  "/manifest.json",
  "/icons/icon-192.png",
  "/icons/icon-512.png"
];
self.addEventListener("install", event => {
  event.waitUntil(
    caches.open(CACHE_NAME)
      .then(cache => cache.addAll(FILES))
      .then(() => self.skipWaiting())
  );
});
self.addEventListener("activate", event => {
  event.waitUntil(
    caches.keys().then(keys =>
      Promise.all(
        keys.filter(key => key !== CACHE_NAME)
          .map(key => caches.delete(key))
      )
    ).then(() => self.clients.claim())
  );
});
self.addEventListener("fetch", event => {
  const url = new URL(event.request.url);
  // Motor commands must never be cached.
  if (url.pathname === "/control") {
    event.respondWith(fetch(event.request, {
      cache: "no-store"
    }));
    return;
  }
  if (event.request.method !== "GET") return;
  event.respondWith(
    caches.match(event.request)
      .then(cached => cached || fetch(event.request))
  );
});
EOF
# Generate icons using macOS built-in tools
if command -v swift >/dev/null 2>&1; then
    cat > "$PROJECT/generate_icons.swift" <<'EOF'
import AppKit
for size in [192, 512] {
    let image = NSImage(size: NSSize(width: size, height: size))
    image.lockFocus()
    let bounds = NSRect(x: 0, y: 0, width: size, height: size)
    NSColor(calibratedRed: 3/255, green: 8/255, blue: 7/255, alpha: 1).setFill()
    bounds.fill()
    let circle = NSBezierPath(ovalIn: bounds.insetBy(dx: size * 0.1, dy: size * 0.1))
    NSColor(calibratedRed: 4/255, green: 37/255, blue: 29/255, alpha: 1).setFill()
    circle.fill()
    NSColor(calibratedRed: 32/255, green: 215/255, blue: 160/255, alpha: 1).setStroke()
    circle.lineWidth = CGFloat(size) / 45
    circle.stroke()
    let knob = NSBezierPath(ovalIn: NSRect(
        x: size * 0.3, y: size * 0.3,
        width: size * 0.4, height: size * 0.4
    ))
    NSColor.white.setFill()
    knob.fill()
    image.unlockFocus()
    if let cgImage = image.cgImage(forProposedRect: nil, context: nil, hints: nil) {
        let bitmap = NSBitmapImageRep(cgImage: cgImage)
        let data = bitmap.representation(using: .png, properties: [:])!
        try! data.write(to: URL(fileURLWithPath: "robot-controller/icons/icon-\(size).png"))
    }
}
EOF
    swift "$PROJECT/generate_icons.swift"
    rm "$PROJECT/generate_icons.swift"
else
    echo "Swift unavailable: generate the two PNG icons manually."
fi
# Add PWA metadata and service worker registration
python3 - "$PROJECT/index.html" <<'PY'
import sys
from pathlib import Path
path = Path(sys.argv[1])
html = path.read_text()
metadata = """
<meta name="theme-color" content="#030807">
<link rel="manifest" href="/manifest.json">
<link rel="icon" href="/icons/icon-192.png">
<link rel="apple-touch-icon" href="/icons/icon-192.png">
"""
if 'rel="manifest"' not in html:
    html = html.replace("</head>", metadata + "\n</head>")
registration = """
<script>
if ("serviceWorker" in navigator) {
  navigator.serviceWorker.register("/service-worker.js")
    .then(() => console.log("PWA service worker registered"))
    .catch(console.error);
}
</script>
"""
if "serviceWorker.register" not in html:
    html = html.replace("</body>", registration + "\n</body>")
path.write_text(html)
PY
echo ""
echo "PWA files created successfully!"
echo "Location: $PROJECT/"
echo ""
echo "Next: configure HTTPS hosting before installing through Chrome."