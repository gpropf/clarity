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

namespace cl2 {

template <typename PixelT>
class GridControl : public std::enable_shared_from_this<GridControl<PixelT>> {
    // val svgDOMElement_ = val::null();
    int gridWidth_, gridHeight_;
    std::string id_;
    std::string svgid_;
    PixelT *pixels_;
    PixelT currentColor_ = 0;
    std::map<PixelT, std::string> colorPallete_;
    shared_ptr<MouseSignal<std::pair<double, double>>> mouseClickSignal_ = nullptr;
    shared_ptr<ObjectAcceptor<std::pair<double, double>, GridControl<PixelT>>>
        svgMouseClickAcceptor_ = nullptr;
    shared_ptr<ObjectAcceptor<std::string, GridControl<PixelT>>> newColorAcceptor_ = nullptr;

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
    // auto mouseSignal = make_shared<MouseSignal<std::pair<double, double>>>(svg, "click");

    //     auto svgMouseClickAcceptor =
    //         make_shared<ObjectAcceptor<std::pair<double, double>, TestObj>>(tobjSptr);
    //     svgMouseClickAcceptor->setSignalAcceptorMethod(&TestObj::mouseAcceptorTestMethod);

    //     sb.connect<std::pair<double, double>>(mouseSignal, svgMouseClickAcceptor);

    void addColorToPallete(PixelT colorValue, const std::string &colorString) {
        colorPallete_[colorValue] = colorString;
    }

    GridControl(int gridWidth, int gridHeight, int pixelWidth, int pixelHeight, SignalBuilder &sb,
                const std::string &id = "", val parentDOMElement = val::null())
        : gridWidth_(gridWidth), gridHeight_(gridHeight) {
        id_ = id;
        svgid_ = this->id_ + "-svg";
        auto svg = cl2::SVG("svg1", pixelWidth, pixelHeight, svgid_, parentDOMElement);
        // svgDOMElement_ = svg.getDomElement();
        svg.setAttributes({{"viewBox", val("0 0 " + std::to_string(gridWidth_) + " " +
                                           std::to_string(gridHeight_))},
                           {"style", val("border: 2px solid blue")}});

        pixels_ = new PixelT[gridWidth_ * gridHeight_];
        initPixels();

        auto colorInput = sb.textInputWSS<std::string>("colorInput", "Enter a number", false);

        mouseClickSignal_ = make_shared<MouseSignal<std::pair<double, double>>>(svg, "click");
        // svgMouseClickAcceptor_ = new RawPointerObjectAcceptor<std::pair<double, double>,
        // GridControl<PixelT>>>();
        svgMouseClickAcceptor_ =
            make_shared<ObjectAcceptor<std::pair<double, double>, GridControl>>();
        svgMouseClickAcceptor_->setSignalAcceptorMethod(&GridControl::mouseAcceptorTestMethod);

        newColorAcceptor_ = make_shared<ObjectAcceptor<std::string, GridControl>>();
        newColorAcceptor_->setSignalAcceptorMethod(&GridControl::setCurrentColor);
        colorInput->setOutput(newColorAcceptor_);

        mouseClickSignal_->setOutput(svgMouseClickAcceptor_);
    }

    /**
     * @brief This must be called to finalize connecting the internal signals. I would do this in
     * the constructor but it seems to cause an exception due to the use of shared_from_this().
     *
     */
    void finalize() {
        this->svgMouseClickAcceptor_->setObjectPointer(this->shared_from_this());
        this->newColorAcceptor_->setObjectPointer(this->shared_from_this());
    }

    /**
     * @brief Better version of mod-in-place method above.
     *
     * @param x
     * @param y
     * @return pair<int,int>
     */
    std::pair<int, int> wrapCoordiates(int x, int y) {
        int xp = x % this->gridWidth_;
        int yp = y % this->gridHeight_;
        if (xp < 0) xp += gridWidth_;
        if (yp < 0) yp += gridHeight_;
        assert(xp >= 0);
        assert(yp >= 0);
        return std::pair(xp, yp);
    }

    void initPixels() {
        int totalPixels = gridHeight_ * gridWidth_;
        while (totalPixels--) {
            pixels_[totalPixels] = 0;
        }
    }

    void printNonZeroPixels() {
        int totalPixels = gridHeight_ * gridWidth_;
        while (totalPixels--) {
            PixelT p = pixels_[totalPixels];
            if (p > 0) cout << "Pixel " << totalPixels << " is " << p << endl;
        }
    }

    void mouseAcceptorTestMethod(const std::pair<double, double> &mouseLocation) {
        cout << "GridControl::mouseAcceptorTestMethod(): x = " << mouseLocation.first
             << ", y = " << mouseLocation.second << endl;

        val document = val::global("document");
        // std::string svgid = this->id_ + "-svg";
        cout << "Looking for svgid: " << svgid_ << endl;
        val svgDOMElement = document.call<val>("getElementById", val(svgid_));
        int floorX = floor(mouseLocation.first);
        int floorY = floor(mouseLocation.second);
        // std::string cursorSquareId = svgid_ + "_" + std::to_string(floorX) + ":" +
        // std::to_string(floorY);
        std::string cursorSquareId = "cursorSquareId";
        std::string colorString = this->colorPallete_[this->currentColor_];
        cout << "Current Color: " << this->currentColor_ << endl;
        auto rect1 =
            Rect("", floorX, floorY, 1, 1, colorString, "", true, cursorSquareId, svgDOMElement);

        pixels_[calculateGridCellIndex(floorX, floorY)] = this->currentColor_;
        printNonZeroPixels();
    }

    void setCurrentColor(const std::string &c) {
        int nc = stoi(c);
        // PixelT *newColor = new PixelT(c);
        this->currentColor_ = nc;
        cout << "GridControl::setCurrentColor(): c = " << this->currentColor_ << endl;
        // this->svgMouseClickAcceptor_->update();
    }
};
}  // namespace cl2
#endif