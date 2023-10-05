/**
 * @file GridControl.hpp
 * @author Greg Propf (gpropf@gmail.com)
 * @brief A Grid control based on the new split signals.
 * @version 0.1
 * @date 2023-03-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef GridControl_hpp
#define GridControl_hpp

#include "Signal.hpp"
#include "SignalBuilder.hpp"
#include "SignalPrimitives.hpp"
#include "Util.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

using std::ostringstream;
using std::string;

namespace cl2 {

template <typename PixelT>
class GridControl : public std::enable_shared_from_this<GridControl<PixelT>> {
    typedef int gridCoordinateT;
    typedef std::pair<gridCoordinateT, gridCoordinateT> gridCoordinatePairT;

    struct FrameSet {
        typedef std::tuple<gridCoordinateT, gridCoordinateT, PixelT> pixelTriplet;
        typedef std::vector<pixelTriplet> frame;

        frame currentFrame_;

        void addPixel(gridCoordinateT x, gridCoordinateT y, PixelT p) {
            pixelTriplet triplet = std::make_tuple(x, y, p);
            currentFrame_.push_back(triplet);
        }

        void makeFrame(bool serialize = false) {
            if (serialize) {
                ostringstream os = serializeFrame(currentFrame_);
                cout << os.str() << endl;
            }
            frames_.push_back(currentFrame_);
            currentFrame_.clear();
        }

        ostringstream serializeFrame(frame fm) {
            ostringstream os;
            for (auto [x, y, p] : fm) {
                os << x << "," << y << "," << int(p) << ";";
            }
            return os;
        }

        std::vector<frame> frames_;
    };
    bool recording_ = false;
    gridCoordinateT gridWidth_, gridHeight_;

    /**
     * @brief Programs that use the GridControl can install their own callbacks here so they can get
     * custom mouse-over behavior.
     *
     */
    std::function<void(double, double)> mousePositionCallback_ = [](double x, double y) {};
    std::string id_;
    std::string svgid_;
    PixelT *pixels_;
    Rect **pixelRects_;
    std::map<PixelT, std::vector<gridCoordinatePairT>> newPixelMap_;
    PixelT currentColor_ = 1;
    std::map<PixelT, std::string> colorPallete_;
    std::function<std::string(PixelT)> pixel2String_ = [](PixelT p) { return std::to_string(p); };
    shared_ptr<MouseSignal<std::pair<double, double>>> mouseClickSignal_ = nullptr;
    shared_ptr<MouseSignal<std::pair<double, double>>> mousePositionSignal_ = nullptr;
    shared_ptr<ObjectAcceptor<std::pair<double, double>, GridControl<PixelT>>>
        svgMouseClickAcceptor_ = nullptr;
    shared_ptr<ObjectAcceptor<std::pair<double, double>, GridControl<PixelT>>>
        svgMousePositionAcceptor_ = nullptr;
    shared_ptr<ObjectAcceptor<std::string, GridControl<PixelT>>> newColorAcceptor_ = nullptr;
    shared_ptr<ObjectEmitter<PixelT, GridControl<PixelT>>> colorEmitter_ = nullptr;
    shared_ptr<CppLambda<PixelT, std::string>> pixel2StringConverter_ = nullptr;
    shared_ptr<SignalBuilder> signalBuilder_ = nullptr;
    FrameSet frameSet_;

   protected:
    int calculateGridCellAddress(int x, int y) const {
        int addr = (y * gridWidth_ + x) * sizeof(PixelT);
        assert(addr >= 0);
        return addr;
    }

    int calculateGridCellIndex(int x, int y) const {
        int addr = (y * gridWidth_ + x);
        assert(addr >= 0);
        return addr;
    }

   public:
    /**
     * @brief Users can use this method to install a custom mouse-over callback.
     *
     * @param callback
     */
    void installMousePositionCallback(std::function<void(double x, double y)> callback) {
        mousePositionCallback_ = callback;
    }

    void addColorToPallete(PixelT colorValue, const std::string &colorString) {
        colorPallete_[colorValue] = colorString;
    }

    PixelT getPixelAt(int x, int y) {
        int pixelIndex = calculateGridCellIndex(x, y);
        return pixels_[pixelIndex];
    }

    void setRecordingOn() {
        recording_ = true;
    }

    void setRecordingOff() {
        recording_ = false;
    }

    /**
     * @brief Set the Pixel at (x,y) and return the value that was there before.
     *
     * @param x
     * @param y
     * @param pixelVal
     * @return PixelT
     */
    PixelT setPixelAt(int x, int y, PixelT pixelVal, bool forceCreateNewRect = false) {
        if (recording_) frameSet_.addPixel(x, y, pixelVal);
        val document = val::global("document");
        val svgDOMElement = document.call<val>("getElementById", val(svgid_));
        int pixelIndex = calculateGridCellIndex(x, y);
        PixelT oldPixelVal = pixels_[pixelIndex];
        pixels_[pixelIndex] = pixelVal;
        // if (redraw) this->redraw();

        std::string colorString = this->colorPallete_[this->pixels_[pixelIndex]];
        // std::string colorString = "#ffff00";

        Rect *rptr = pixelRects_[pixelIndex];

        if (rptr == nullptr || forceCreateNewRect) {
            auto *rectptr =
                new Rect("", x, y, 1, 1, colorString, "teal", 0.1, false, "", svgDOMElement);
            pixelRects_[pixelIndex] = rectptr;
        } else {
            rptr->domElement_.call<void>("setAttribute", val("fill"), val(colorString));
        }

        auto pos = std::make_pair(x, y);
        // newPixelMap_[pixelVal].push_back(pos);
        if (pixelVal != oldPixelVal) newPixelMap_[pixelVal].push_back(pos);
        return oldPixelVal;
    }

    void makeFrame(bool serialize = false) { frameSet_.makeFrame(serialize); }

    GridControl(int gridWidth, int gridHeight, int pixelWidth, int pixelHeight,
                shared_ptr<SignalBuilder> signalBuilder, const std::string &id = "",
                val parentDOMElement = val::null())
        : gridWidth_(gridWidth), gridHeight_(gridHeight), signalBuilder_(signalBuilder) {
        id_ = id;
        pixelRects_ = new Rect *[gridWidth_ * gridHeight_];
        svgid_ = this->id_ + "-svg";
        auto svg = signalBuilder_->svg("svg1", pixelWidth, pixelHeight, svgid_, parentDOMElement);
        // svgDOMElement_ = svg.getDomElement();
        SignalBuilder gcsb = signalBuilder->withAttributes({{"class", val("small_width")}}, false);
        auto br = cl2::BR();
        // cl2::BR();
        // br.forceDeleteDomElementOnExit();
        // br.forceDeleteDomElementOnExit();
        svg.setAttributes({{"viewBox", val("0 0 " + std::to_string(gridWidth_) + " " +
                                           std::to_string(gridHeight_))},
                           {"style", val("border: 2px solid blue")},
                           {"preserveAspectRatio", val("none")}});

        pixels_ = new PixelT[gridWidth_ * gridHeight_];
        clearGridToValue(0, true);

        auto xyPairToString = [](std::pair<double, double> p) {
            std::string xstr = std::to_string(p.first);
            std::string ystr = std::to_string(p.second);
            return xstr + ":" + ystr;
        };

        // We now place our lambda in the core of the CppLambda signal wrapper.
        // auto strToNumTransformer = make_shared<cl2::CppLambda<std::string, double>>(str2DblFn);
        auto xyPairToStringTrn =
            make_shared<cl2::CppLambda<std::pair<double, double>, std::string>>(xyPairToString);

        auto colorInput = gcsb.textInputWSS<std::string>("colorInput", "Color Index", false);

        mouseClickSignal_ = make_shared<MouseSignal<std::pair<double, double>>>(svg, "click");

        mousePositionSignal_ =
            make_shared<MouseSignal<std::pair<double, double>>>(svg, "mousemove");
        // mouseClickSignal_ = make_shared<MouseSignal<std::pair<double, double>>>(svg,
        // "mouseover");

        // svgMouseClickAcceptor_ = new RawPointerObjectAcceptor<std::pair<double, double>,
        // GridControl<PixelT>>>();
        svgMouseClickAcceptor_ =
            make_shared<ObjectAcceptor<std::pair<double, double>, GridControl>>();
        svgMouseClickAcceptor_->setSignalAcceptorMethod(&GridControl::mouseClickAcceptorMethod);

        svgMousePositionAcceptor_ =
            make_shared<ObjectAcceptor<std::pair<double, double>, GridControl>>();
        svgMousePositionAcceptor_->setSignalAcceptorMethod(&GridControl::mousePositionAcceptor);

        newColorAcceptor_ = make_shared<ObjectAcceptor<std::string, GridControl>>();
        newColorAcceptor_->setSignalAcceptorMethod(&GridControl::setCurrentColor);

        colorEmitter_ = make_shared<ObjectEmitter<PixelT, GridControl>>();
        colorEmitter_->setSignalEmitterMethod(&GridControl::getCurrentColor);

        pixel2StringConverter_ = make_shared<cl2::CppLambda<PixelT, std::string>>(pixel2String_);

        colorEmitter_->setOutput(pixel2StringConverter_);
        pixel2StringConverter_->setOutput(colorInput);
        colorInput->setOutput(newColorAcceptor_);

        mouseClickSignal_->setOutput(svgMouseClickAcceptor_);
        mousePositionSignal_->setOutput(svgMousePositionAcceptor_);
        // svgMousePositionAcceptor_->setOutput(xyPairToStringTrn);
    }

    /**
     * @brief This must be called to finalize connecting the internal signals. I would do this in
     * the constructor but it seems to cause an exception due to the use of shared_from_this().
     *
     */
    void finalize() {
        this->svgMouseClickAcceptor_->setObjectPointer(this->shared_from_this());
        // this->signalBuilder_->connect<std::pair<double, double>>(this->svgMouseClickAcceptor_,
        // this->shared_from_this());

        this->svgMousePositionAcceptor_->setObjectPointer(this->shared_from_this());
        this->newColorAcceptor_->setObjectPointer(this->shared_from_this());
        this->colorEmitter_->setObjectPointer(this->shared_from_this());
        // colorEmitter_->setOutput(pixel2StringConverter_);
        this->colorEmitter_->update();
        this->svgMouseClickAcceptor_->update();
        this->svgMousePositionAcceptor_->update();
        this->newColorAcceptor_->update();
    }

    /**
     * @brief Better version of mod-in-place method above.
     *
     * @param x
     * @param y
     * @return pair<int,int>
     */
    // std::pair<int, int> wrapCoordiates(int x, int y) {
    //     int xp = x % this->gridWidth_;
    //     int yp = y % this->gridHeight_;
    //     if (xp < 0) xp += gridWidth_;
    //     if (yp < 0) yp += gridHeight_;
    //     assert(xp >= 0);
    //     assert(yp >= 0);
    //     return std::pair(xp, yp);
    // }

    void clearGridToValue(PixelT v = 0, bool forceCreateNewRect = false) {
        int totalPixels = gridHeight_ * gridWidth_;
        while (totalPixels--) {
            pixels_[totalPixels] = v;
        }
        clearNewPixelMap();
        this->redraw(forceCreateNewRect);
    }

    void printNonZeroPixels(bool convertToIntBeforePrinting = true) {
        int totalPixels = gridHeight_ * gridWidth_;
        while (totalPixels--) {
            PixelT p = pixels_[totalPixels];

            if (p > 0) {
                if (convertToIntBeforePrinting)
                    cout << "Pixel " << totalPixels << " is " << int(p) << endl;
                else
                    cout << "Pixel " << totalPixels << " is " << p << endl;
            }
        }
    }

    void printNewPixels() {
        cout << "NEW PIXELS #7:" << endl;
        for (const auto &[color, pixels] : newPixelMap_) {
            cout << "COLOR: " << int(color) << endl;
            for (const auto &p : pixels) {
                cout << "\t" << p.first << ": " << p.second << endl;
            }
        }
    }

    void redraw(bool forceCreateNewRect = false) {
        // val document = val::global("document");
        // val svgDOMElement = document.call<val>("getElementById", val(svgid_));

        for (int i = 0; i < this->gridWidth_; i++) {
            for (int j = 0; j < this->gridHeight_; j++) {
                int idx = calculateGridCellIndex(i, j);
                // std::string colorString = this->colorPallete_[this->pixels_[idx]];
                // // std::string colorString = "#ffff00";
                setPixelAt(i, j, this->pixels_[idx], forceCreateNewRect);
                // if (pixelRects_[idx] == nullptr) {
                //     auto *rectptr = new Rect("", i, j, 1, 1, colorString, "teal", 0.1, false, "",
                //                              svgDOMElement);
                //     pixelRects_[idx] = rectptr;
                // }
                // else {
                //     pixelRects_[idx]->domElement_.call<void>("setAttribute", val("fill"),
                //     val(colorString));
                // }
            }
        }
    }

    std::map<PixelT, std::vector<gridCoordinatePairT>> getNewPixelMap() { return newPixelMap_; }

    void clearNewPixelMap() { newPixelMap_.clear(); }

    void mousePositionAcceptor(const std::pair<double, double> &mouseLocation) {
        // cout << "GridControl::mousePositionAcceptor(): x = " << floor(mouseLocation.first)
        //      << ", y = " << floor(mouseLocation.second) << endl;
        mousePositionCallback_(mouseLocation.first, mouseLocation.second);
    }

    void mouseClickAcceptorMethod(const std::pair<double, double> &mouseLocation) {
        // cout << "GridControl::mouseClickAcceptorMethod(): x = " << mouseLocation.first
        //      << ", y = " << mouseLocation.second << endl;

        val document = val::global("document");
        val svgDOMElement = document.call<val>("getElementById", val(svgid_));
        int floorX = floor(mouseLocation.first);
        int floorY = floor(mouseLocation.second);
        std::string cursorSquareId = "cursorSquareId";
        std::string colorString = this->colorPallete_[this->currentColor_];
        setPixelAt(floorX, floorY, currentColor_);
    }

    void setCurrentColor(const std::string &c) {
        int nc = stoi(c);
        // PixelT *newColor = new PixelT(c);
        this->currentColor_ = nc;
        cout << "GridControl::setCurrentColor(): c = " << this->currentColor_ << endl;
        // this->svgMouseClickAcceptor_->update();
    }

    PixelT getCurrentColor() { return this->currentColor_; }
};
}  // namespace cl2
#endif