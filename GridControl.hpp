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
class GridControl {  //: public std::enable_shared_from_this<GridControl<PixelT>> {
    int gridWidth_, gridHeight_;
    PixelT *pixels_;
    PixelT currentColor_;
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

   public:
    // auto mouseSignal = make_shared<MouseSignal<std::pair<double, double>>>(svg, "click");

    //     auto svgMouseClickAcceptor =
    //         make_shared<ObjectAcceptor<std::pair<double, double>, TestObj>>(tobjSptr);
    //     svgMouseClickAcceptor->setSignalAcceptorMethod(&TestObj::mouseAcceptorTestMethod);

    //     sb.connect<std::pair<double, double>>(mouseSignal, svgMouseClickAcceptor);

    GridControl(int gridWidth, int gridHeight, int pixelWidth, int pixelHeight, SignalBuilder &sb,
                const std::string &id = "", val parentDOMElement = val::null())
        : gridWidth_(gridWidth), gridHeight_(gridHeight) {
        auto svg = cl2::SVG("svg1", pixelWidth, pixelHeight, id, parentDOMElement);
        svg.setAttributes({{"viewBox", val("0 0 " + std::to_string(gridWidth_) + " " +
                                           std::to_string(gridHeight_))},
                           {"style", val("border: 2px solid blue")}});

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
        // svgMouseClickAcceptor_->setObjectPointer(this->shared_from_this());
        mouseClickSignal_->setOutput(svgMouseClickAcceptor_);
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

    void mouseAcceptorTestMethod(const std::pair<double, double> &mouseLocation) {
        cout << "GridControl::mouseAcceptorTestMethod(): x = " << mouseLocation.first
             << ", y = " << mouseLocation.second << endl;
    }

    void setCurrentColor(const std::string &c) {
        cout << "GridControl::mouseAcceptorTestMethod(): c = " << c << endl;
    }
};
}  // namespace cl2
#endif