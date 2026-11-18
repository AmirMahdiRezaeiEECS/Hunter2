import AppKit

func generateIcon(size: Int, outputPath: String) {
    let canvasSize = CGFloat(size)

    let image = NSImage(size: NSSize(
        width: canvasSize,
        height: canvasSize
    ))

    image.lockFocus()

    let bounds = NSRect(
        x: 0,
        y: 0,
        width: canvasSize,
        height: canvasSize
    )

    // Background
    NSColor(calibratedRed: 4/255, green: 37/255, blue: 29/255, alpha: 1).setFill()
    bounds.fill()

    // Main circle
    let circleBounds = bounds.insetBy(
        dx: canvasSize * 0.1,
        dy: canvasSize * 0.1
    )

    let circle = NSBezierPath(ovalIn: circleBounds)

    NSColor.white.setStroke()
    circle.lineWidth = canvasSize * 0.025
    circle.stroke()

    // Center knob
    let knobRect = NSRect(
        x: canvasSize * 0.3,
        y: canvasSize * 0.3,
        width: canvasSize * 0.4,
        height: canvasSize * 0.4
    )

    let knob = NSBezierPath(ovalIn: knobRect)

    NSColor.white.setFill()
    knob.fill()

    image.unlockFocus()

    guard
        let tiffData = image.tiffRepresentation,
        let bitmap = NSBitmapImageRep(data: tiffData),
        let pngData = bitmap.representation(using: .png, properties: [:])
    else {
        print("Failed to generate icon.")
        return
    }

    do {
        try pngData.write(to: URL(fileURLWithPath: outputPath))
        print("Generated: \(outputPath)")
    } catch {
        print("Error: \(error)")
    }
}

generateIcon(size: 192, outputPath: "robot-controller/icons/icon-192.png")
generateIcon(size: 512, outputPath: "robot-controller/icons/icon-512.png")