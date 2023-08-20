#ifndef Pixelreactor_hpp
#define Pixelreactor_hpp

#include <set>
#include <utility>

#include "GridControl.hpp"
#include "Signal.hpp"
#include "SignalBuilder.hpp"
#include "SignalPrimitives.hpp"
#include "Util.hpp"
#include "WebElementSignals.hpp"
#include "WebElements.hpp"

using namespace cl2;

/**
 * @brief My own stringification function once I saw that to_string() wasn't always working.
 *
 * @tparam T
 * @param v
 * @return std::string
 */
template <typename T>
std::string clto_str(const T &v) {
    std::ostringstream os;
    os << v;
    return os.str();
}

template <typename V>
struct RotationMatrix2D {
    const V r1c1_, r1c2_, r2c1_, r2c2_;
    const int angle_;

    RotationMatrix2D(V r1c1, V r1c2, V r2c1, V r2c2, int angle)
        : r1c1_(r1c1), r1c2_(r1c2), r2c1_(r2c1), r2c2_(r2c2), angle_(angle) {}

    std::pair<V, V> rotateCoordinates(std::pair<V, V> coords) const {
        auto [x, y] = coords;
        V xprime = r1c1_ * x + r1c2_ * y;
        V yprime = r2c1_ * x + r2c2_ * y;
        return std::pair(xprime, yprime);
    }

    // static const RotationMatrix2D<V> rr0; // = RotationMatrix2D<V>(1, 0, 0, 1, 0);
};

const RotationMatrix2D r0 = RotationMatrix2D(1, 0, 0, 1, 0);
const RotationMatrix2D r90 = RotationMatrix2D(0, -1, 1, 0, 90);
const RotationMatrix2D r180 = RotationMatrix2D(-1, 0, 0, -1, 180);
const RotationMatrix2D r270 = RotationMatrix2D(0, 1, -1, 0, 270);

// template <typename U>
// class Beaker;

/**
 * @brief Represents a single "reaction vessel" in which our experiments can take place. The
 * reaction rules that determine how patterns in the grid transform will use the same GridControl
 * control the beaker itself does.
 *
 * @tparam V This is the type we are using for the grid elements. The original app I wrote in
 * ClojureScript used small positive integers for the colors so the expected type here is `unsigned
 * char`. Theoretically, it's possible to use other types though.
 */
template <typename V>
class Beaker : public std::enable_shared_from_this<Beaker<V>> {
   public:
    typedef short int
        priorityT;  // I'm using an int type for the text input field for priorities because the
                    // values weren't "sticking" otherwise. This is a type related bug.
    typedef std::pair<V, priorityT> valuePriorityPairT;
    typedef int gridCoordinateT;
    typedef std::pair<gridCoordinateT, gridCoordinateT> gridCoordinatePairT;
    typedef std::pair<gridCoordinatePairT, V> gridCoordinatesValueTripletT;

    std::vector<gridCoordinatePairT> matchLists[4];

    RotationMatrix2D<gridCoordinateT> *r0__, *r90__, *r180__,
        *r270__;  //!< I tried to make these static const class members because they're the same for
                  //!< all Beakers but ran into huge problems getting link errors and a lot of
                  //!< errors about forward template definitions. The small space and time savings
                  //!< wasn't worth it.

    /**
     * @brief Adapted from this doc:
     * https://en.cppreference.com/w/cpp/memory/enable_shared_from_this
     *
     * @return std::shared_ptr<Beaker<V>>
     */
    std::shared_ptr<Beaker<V>> getptr() { return this->shared_from_this(); }

    /**
     * @brief Needs to be called outside the constructor due to use of getptr()
     *
     */
    void finalize() {
        if (isReactionRule_) {
            nameInput_ = signalBuilder_->withAttributes({{"class", val("medium_width")}})
                             .textInputWSS<std::string>("nameInput", "Rule Name", false);

            nameInputLoop_ = make_shared<ObjectSignalLoop<std::string, Beaker<unsigned char>>>(
                getptr(), nameInput_, &Beaker<unsigned char>::setName,
                &Beaker<unsigned char>::getName);

            signalBuilder_->connectLoop(nameInputLoop_);
        } else {
            objAcceptor_ = make_shared<ObjectAcceptor<std::string, Beaker<V>>>(getptr());
            objAcceptor_->setSignalAcceptorMethod(&Beaker::makeNewReactionRuleSignal);
            newRuleButton_ = signalBuilder_->buttonWSS<std::string>("New Rule WSS");
            signalBuilder_->connect<std::string>(newRuleButton_, objAcceptor_);

            iterateAcceptor_ = make_shared<ObjectAcceptor<std::string, Beaker<V>>>(getptr());
            iterateAcceptor_->setSignalAcceptorMethod(&Beaker::iterate);
            iterateButton_ = signalBuilder_->buttonWSS<std::string>("Iterate");
            signalBuilder_->connect<std::string>(iterateButton_, iterateAcceptor_);
        }
    }

    void makeNewReactionRuleSignal(const std::string &s) {
        cout << "GOT SIGNAL FROM BUTTON: " << s << endl;
        this->makeNewReactionRule();
    }

    Beaker(shared_ptr<cl2::SignalBuilder> signalBuilder, int gridWidth, int gridHeight,
           int gridPixelWidth, int gridPixelHeight, const std::string &name = "",
           bool isReactionRule = false)
        : signalBuilder_(signalBuilder),
          gridWidth_(gridWidth),
          gridHeight_(gridHeight),
          gridPixelWidth_(gridPixelWidth),
          gridPixelHeight_(gridPixelHeight),
          name_(name),
          isReactionRule_(isReactionRule) {
        initStandardRotationMatrices();
        cout << "Beaker created!" << endl;

        // SignalBuilder &sb = *signalBuilder_;
        gridControl_ =
            make_shared<GridControl<V>>(gridWidth_, gridHeight_, gridPixelWidth_, gridPixelHeight_,
                                        signalBuilder_, name_ + "_gc1");
        gridControl_->addColorToPallete(0, "#000000");
        gridControl_->addColorToPallete(1, "#ff0000");
        gridControl_->addColorToPallete(2, "#00ff00");
        gridControl_->addColorToPallete(3, "#0000ff");
        gridControl_->finalize();
    }

    void setRuleGridWidth(const std::string &v) {
        ruleGridWidth_ = std::stoi(v);
        cout << "SETTING ruleGridWidth_ to " << v << endl;
    }

    std::string getRuleGridWidth() {
        std::string rgwString = std::to_string(ruleGridWidth_);
        cout << "GETTING ruleGridWidth_ as string value: '" << rgwString << "'" << endl;
        return rgwString;
    }

    void setRuleGridHeight(const std::string &v) {
        ruleGridHeight_ = std::stoi(v);
        cout << "SETTING ruleGridHeight_ to " << v << endl;
    }

    std::string getRuleGridHeight() {
        std::string outstr = std::to_string(ruleGridHeight_);
        cout << "GETTING ruleGridHeight_ as string value: '" << outstr << "'" << endl;
        return outstr;
    }

    void setName(const std::string &v) {
        name_ = v;
        cout << "SETTING name_ to " << v << endl;
    }

    std::string getName() {
        cout << "GETTING name_ : '" << name_ << "'" << endl;
        return name_;
    }

    /**
     * @brief Creates a new smaller Beaker to serve as a reaction pattern to run in the main
     * grid.
     *
     */
    void makeNewReactionRule() {
        shared_ptr<Beaker<unsigned char>> reactionRule = make_shared<Beaker<unsigned char>>(
            this->signalBuilder_, this->ruleGridWidth_, this->ruleGridHeight_, 150, 100,
            "rule" + clto_str(++this->ruleCount_), true);
        reactionRule->parentBeaker_ = this;
        this->reactionRules_.push_back(reactionRule);
        reactionRule->finalize();

        // nameInput_ =
        //     signalBuilder_->withAttributes({{"class", val("medium_width")}})
        //         .textInputWSS<std::string>("nameInput", "Rule Name", false);

        // reactionRule->nameInputLoop_ = make_shared<ObjectSignalLoop<std::string, Beaker<unsigned
        // char>>>(
        //     reactionRule, reactionRule->nameInput_, &Beaker<unsigned char>::setName,
        //     &Beaker<unsigned char>::getName);

        // signalBuilder_->connectLoop(reactionRule->nameInputLoop_);
    }

    void wrapCoordinates(gridCoordinatePairT &p) {
        if (p.first < 0)
            p.first = gridWidth_ + p.first;
        else
            p.first = p.first % gridWidth_;
        if (p.second < 0)
            p.second = gridHeight_ + p.second;
        else
            p.second = p.second % gridHeight_;
        // if (p.first < 0 || p.second < 0) cout << "NEGATIVE COORDS!!!!!!!!!" << endl;
    }

    bool matchAt(shared_ptr<Beaker<V>> beaker, gridCoordinateT x, gridCoordinateT y,
                 const RotationMatrix2D<gridCoordinateT> &rotationMatrix) {
        for (gridCoordinateT i = 0; i < beaker->gridWidth_; i++) {
            for (gridCoordinateT j = 0; j < beaker->gridHeight_; j++) {
                V ruleVal = beaker->gridControl_->getPixelAt(i, j);

                gridCoordinatePairT gridOffset = std::make_pair(i, j);
                gridOffset = rotationMatrix.rotateCoordinates(gridOffset);

                gridCoordinateT gx = x + gridOffset.first;
                gridCoordinateT gy = y + gridOffset.second;
                gridCoordinatePairT gp = std::make_pair(gx, gy);
                wrapCoordinates(gp);
                V gridVal = this->gridControl_->getPixelAt(gp.first, gp.second);
                // cout << i << ", " << j << ":: " << int(ruleVal) << "|| " << gp.first << ", "
                //      << gp.second << ":: " << int(gridVal) << endl;
                if (gridVal != ruleVal) return false;
            }
        }
        return true;
    }

    void populateMatchLists() {
        if (isReactionRule_) return;
        for (auto reactionRule : reactionRules_) {
            cout << "update() BEGIN Reaction rule address: " << reactionRule << endl;
            for (gridCoordinateT i = 0; i < this->gridWidth_; i++) {
                for (gridCoordinateT j = 0; j < this->gridHeight_; j++) {
                    if (matchAt(reactionRule, i, j, r0)) {
                        // cout << "(r = 0) MATCH: " << i << ", " << j << endl;
                        reactionRule->matchLists[0].push_back(std::make_pair(i, j));
                    }
                    if (matchAt(reactionRule, i, j, r90)) {
                        // cout << "(r = 90) MATCH: " << i << ", " << j << endl;
                        reactionRule->matchLists[1].push_back(std::make_pair(i, j));
                    }
                    if (matchAt(reactionRule, i, j, r180)) {
                        // cout << "(r = 180) MATCH: " << i << ", " << j << endl;
                        reactionRule->matchLists[2].push_back(std::make_pair(i, j));
                    }
                    if (matchAt(reactionRule, i, j, r270)) {
                        // cout << "(r = 270) MATCH: " << i << ", " << j << endl;
                        reactionRule->matchLists[3].push_back(std::make_pair(i, j));
                    }
                }
            }
        }
    }

    void printMatchLists() {
        for (auto reactionRule : reactionRules_) {
            cout << "Reaction rule '" << reactionRule->name_ << "'" << endl;
            for (int i = 0; i < 4; i++) {
                for (const auto &[x, y] : reactionRule->matchLists[i]) {
                    cout << "Rot: " << i * 90 << " - " << x << ", " << y << endl;
                }
            }
        }
    }

    void purgeMatchLists() {
        for (auto reactionRule : reactionRules_) {
            for (int i = 0; i < 4; i++) {
                reactionRule->matchLists[i].clear();
            }
        }
    }

    /**
     * @brief Performs basic hygeine functions to maintain the lists that are used in matching and
     * iteration.
     *
     */
    void update() {
        populateMatchLists();
        printMatchLists();
        purgeMatchLists();
    }

    /**
     * <<< OLD CODE BELOW THIS LINE
     */

    Beaker<unsigned char> *findRuleByName(const std::string &ruleName) {
        auto nameIs = [&ruleName](Beaker *b) { return (b->name_ == ruleName); };
        auto it = find_if(reactionRules_.begin(), reactionRules_.end(), nameIs);
        if (it == reactionRules_.end()) return nullptr;
        return *it;
    }

    void initStandardRotationMatrices() {
        r0__ = new RotationMatrix2D<gridCoordinateT>(1, 0, 0, 1, 0);
        r90__ = new RotationMatrix2D<gridCoordinateT>(0, -1, 1, 0, 90);
        r180__ = new RotationMatrix2D<gridCoordinateT>(-1, 0, 0, -1, 180);
        r270__ = new RotationMatrix2D<gridCoordinateT>(0, 1, -1, 0, 270);
    }

    void initPixelListMap() {
        std::map<V, std::vector<gridCoordinatePairT>> valueToCoordinateMap0;
        std::map<V, std::vector<gridCoordinatePairT>> valueToCoordinateMap90;
        std::map<V, std::vector<gridCoordinatePairT>> valueToCoordinateMap180;
        std::map<V, std::vector<gridCoordinatePairT>> valueToCoordinateMap270;

        rotationToPixelListsMap_[0] = valueToCoordinateMap0;
        rotationToPixelListsMap_[90] = valueToCoordinateMap90;
        rotationToPixelListsMap_[180] = valueToCoordinateMap180;
        rotationToPixelListsMap_[270] = valueToCoordinateMap270;
    }

    void addPixelToRotationMaps(gridCoordinatePairT coordinates, V pixelValue) {
        rotationToPixelListsMap_[0][pixelValue].push_back(coordinates);
        gridCoordinatePairT coordinates90 = r90__->rotateCoordinates(coordinates);
        rotationToPixelListsMap_[90][pixelValue].push_back(coordinates90);
        gridCoordinatePairT coordinates180 = r180__->rotateCoordinates(coordinates);
        rotationToPixelListsMap_[180][pixelValue].push_back(coordinates180);
        gridCoordinatePairT coordinates270 = r270__->rotateCoordinates(coordinates);
        rotationToPixelListsMap_[270][pixelValue].push_back(coordinates270);
    }

    /**
     * @brief Warning: doesn't sort yet.
     *
     * @param m
     * @return std::vector<std::pair<V, std::vector<gridCoordinatePairT>>>
     */
    std::vector<std::pair<V, std::vector<gridCoordinatePairT>>> sortPixelList(
        std::map<V, std::vector<gridCoordinatePairT>> &m) {
        std::vector<std::pair<V, std::vector<gridCoordinatePairT>>> vectorOfPairs;
        for (const auto &[pixelVal, coordinates] : m) {
            vectorOfPairs.push_back(std::pair(pixelVal, coordinates));
        }

        return vectorOfPairs;
    }

    void sortPixelLists() {
        rotationToSortedPixelListsMap_[0] = sortPixelList(rotationToPixelListsMap_[0]);
        rotationToSortedPixelListsMap_[90] = sortPixelList(rotationToPixelListsMap_[90]);
        rotationToSortedPixelListsMap_[180] = sortPixelList(rotationToPixelListsMap_[180]);
        rotationToSortedPixelListsMap_[270] = sortPixelList(rotationToPixelListsMap_[270]);
    }

    void makePixelList() {
        rotationToPixelListsMap_.clear();
        rotationToSortedPixelListsMap_.clear();
        std::vector<gridCoordinatesValueTripletT> pixels;
        for (gridCoordinateT i = 0; i < this->gridWidth_; i++) {
            for (gridCoordinateT j = 0; j < this->gridHeight_; j++) {
                V pixelVal = this->gridControl_->getPixelAt(i, j);
                cout << i << ", " << j << ": " << pixelVal << endl;
                gridCoordinatePairT xy = std::pair(i, j);
                gridCoordinatesValueTripletT xyv;
                if (this->isReactionRule_) {
                    // addPixelToRotationMaps(xy, pixelVal);
                }
            }
        }
    }

    /**
     * @brief Just a debug method to see if the successor setting code works.
     *
     */
    void printRuleStats() {
        int i = 0;
        for (auto rule : this->reactionRules_) {
            cout << "Rule " << i++ << ": " << rule->name_ << ", succ: " << rule->successor_->name_
                 << endl;
        }
    }

    void printPixelList(std::vector<std::pair<V, std::vector<gridCoordinatePairT>>> &pl) {
        cout << "PPL() List address: " << &pl << ", rule name: " << name_ << endl;
        for (auto p : pl) {
            auto [pixelVal, gridCoordinates] = p;
            cout << "PPL() For pixelVal " << int(pixelVal) << endl;
            for (auto coordinatePair : gridCoordinates) {
                cout << "\tPPL() " << coordinatePair.first << ", " << coordinatePair.second << endl;
            }
        }
    }

    void printPixelMap(std::map<V, std::vector<gridCoordinatePairT>> &m) {
        cout << "PPM() rule name: " << name_ << endl;
        for (auto [pixelVal, gridCoordinates] : m) {
            cout << "PPM() For pixelVal " << int(pixelVal) << endl;
            for (auto coordinatePair : gridCoordinates) {
                cout << "\tPPM() " << coordinatePair.first << ", " << coordinatePair.second << endl;
            }
        }
    }

    bool matchList2(std::vector<gridCoordinatePairT> &rulePixelList,
                    gridCoordinatePairT matchCoordiates, V pixelVal) {
        auto [mx, my] = matchCoordiates;

        for (auto pixel : rulePixelList) {
            auto [px, py] = pixel;
            px += mx;
            py += my;
            auto [ppx, ppy] = this->beakerNode_->beakerCanvas_->wrapCoordiates(px, py);
            V mainGridVal = this->beakerNode_->beakerCanvas_->getValXY(ppx, ppy);
            if (mainGridVal != pixelVal) return false;
        }
        return true;
    }

    bool permutatedMatchList(std::vector<gridCoordinatePairT> &rulePixelList,
                             gridCoordinatePairT matchCoordiates, V pixelVal, int anchorIndex) {
        auto [mx, my] = matchCoordiates;
        auto [ax, ay] = rulePixelList.at(anchorIndex);
        auto rulePixelListSize = rulePixelList.size();
        for (auto i = 0; i < rulePixelListSize; i++) {
            if (i == anchorIndex) continue;
            auto [px, py] = rulePixelList.at(i);
            px += mx - ax;
            py += my - ay;
            auto [ppx, ppy] = this->beakerNode_->beakerCanvas_->wrapCoordiates(px, py);
            V mainGridVal = this->beakerNode_->beakerCanvas_->getValXY(ppx, ppy);
            if (mainGridVal != pixelVal) return false;
        }
        return true;
    }

    bool matchesAtByRotation(Beaker<V> &rule, RotationMatrix2D<gridCoordinateT> *rotation,
                             gridCoordinatePairT matchCoordiates) {
        std::vector<std::pair<V, std::vector<gridCoordinatePairT>>> pixelLists =
            rule.rotationToSortedPixelListsMap_[rotation->angle_];
        bool couldMatch = false;
        for (auto [pixelVal, coordinates] : pixelLists) {
            couldMatch = matchList2(coordinates, matchCoordiates, pixelVal);
            if (!couldMatch) return false;
        }
        return couldMatch;
    }

    std::vector<gridCoordinatePairT> generatePotentialMatchCoordinates(
        std::tuple<gridCoordinateT, gridCoordinateT, V> &newPixel,
        std::map<V, std::vector<gridCoordinatePairT>> &valueToPixelLocationsMap) const {
        auto [npx, npy, npVal] = newPixel;
        std::vector<gridCoordinatePairT> coordinatesVector = valueToPixelLocationsMap[npVal];
        std::vector<gridCoordinatePairT> potentialMatchCoordinates;
        for (auto [rx, ry] : coordinatesVector) {
            auto pmx = npx - rx;
            auto pmy = npy - ry;
            potentialMatchCoordinates.push_back(std::pair(pmx, pmy));
        }
        return potentialMatchCoordinates;
    }

    void matchAndSucceed(Beaker<unsigned char> &reactionRule, gridCoordinatePairT gridCoordinates) {
        bool mbr = matchesAtByRotation(reactionRule, r0__, gridCoordinates);
        auto [i, j] = gridCoordinates;

        if (mbr) {
            cout << "Using matchesAtByRotation() there is a match at " << i << ", " << j
                 << " for 0 degree rotatation." << endl;
            laydownMatchPixels2(reactionRule, gridCoordinates, r0__);
        }

        mbr = matchesAtByRotation(reactionRule, r90__, gridCoordinates);

        if (mbr) {
            cout << "Using matchesAtByRotation() there is a match at " << i << ", " << j
                 << " for 90 degree rotatation." << endl;
            laydownMatchPixels2(reactionRule, gridCoordinates, r90__);
        }

        mbr = matchesAtByRotation(reactionRule, r180__, gridCoordinates);

        if (mbr) {
            cout << "Using matchesAtByRotation() there is a match at " << i << ", " << j
                 << " for 180 degree rotatation." << endl;
            laydownMatchPixels2(reactionRule, gridCoordinates, r180__);
        }

        mbr = matchesAtByRotation(reactionRule, r270__, gridCoordinates);

        if (mbr) {
            cout << "Using matchesAtByRotation() there is a match at " << i << ", " << j
                 << " for 270 degree rotatation." << endl;
            laydownMatchPixels2(reactionRule, gridCoordinates, r270__);
        }
    }

    void clearGrid() {
        cout << "clearGrid()" << endl;
        // this->beakerNode_->beakerCanvas_->clearGridToValue(0);
    }

    void laydownMatchPixels2(Beaker<V> &reactionRule, gridCoordinatePairT matchCoordiates,
                             RotationMatrix2D<gridCoordinateT> *rotation) {
        auto [mx, my] = matchCoordiates;
        gridCoordinatePairT successorOffset =
            std::pair(reactionRule.successorOffsetX_, reactionRule.successorOffsetY_);
        successorOffset = rotation->rotateCoordinates(successorOffset);
        auto [sox, soy] = successorOffset;
        int c = 0;
        for (auto vectorOfPairs :
             reactionRule.successor_->rotationToSortedPixelListsMap_[rotation->angle_]) {
            auto [pixelVal, gridCoordinates] = vectorOfPairs;
            for (auto [px, py] : gridCoordinates) {
                c++;
                px += sox + mx;
                py += soy + my;
                auto [ppx, ppy] = this->beakerNode_->beakerCanvas_->wrapCoordiates(px, py);
                valuePriorityPairT vp = std::pair(pixelVal, reactionRule.successionPriority_);
                successionMap_[std::pair(ppx, ppy)].push_back(vp);
            }
        }
        cout << "Done laying down " << c << " match pixels for rule: " << reactionRule.name_
             << endl;
    }

    void updateGrid() {
        clean_ = false;

        for (const auto &[key, value] : this->successionMap_) {
            auto [px, py] = key;
            std::vector<valuePriorityPairT> vpStack = value;
            cout << "update coordinate: " << px << ", " << py << endl;
            if (!vpStack.empty()) {
                sortValuePriorityStack(vpStack);
                auto [val, pri] = vpStack.back();
                // this->beakerNode_->beakerCanvas_->setValXYNoDraw(px, py, val);
            }
        }
    }

    static bool compareValuePriorityPairs(valuePriorityPairT vp1, valuePriorityPairT vp2) {
        return (vp1.second < vp2.second);
    }

    void sortValuePriorityStack(std::vector<valuePriorityPairT> &vpStack) const {
        sort(vpStack.begin(), vpStack.end(), compareValuePriorityPairs);
    }

    bool toggleClean() {
        clean_ = !clean_;
        cout << "BEAKER CLEAN STATUS = " << clean_ << endl;
        return clean_;
    }

    void makeDirty() {
        clean_ = false;
        cout << "BEAKER IS DIRTY!" << endl;
        // auto [x, y, pixelVal] = this->beakerNode_->beakerCanvas_->getLatestPixel();
        // cout << "LATEST PIXEL: " << x << ", " << y << " : " << int(pixelVal) << endl;
    }

    void multiMatch(Beaker<V> &reactionRule, RotationMatrix2D<gridCoordinateT> *rm) {
        std::set<gridCoordinatePairT> uniqueMatchLocations;
        // for (auto [x, y, pixelVal] : this->beakerNode_->beakerCanvas_->pixelBuffer_) {
        //     cout << "NEW PIXEL: " << x << ", " << y << " : " << int(pixelVal) << endl;
        //     auto newPixel = make_tuple(x, y, pixelVal);
        //     auto valueToPixelLocationsMap = reactionRule.rotationToPixelListsMap_[rm->angle_];
        //     auto potentialMatchCoordinates =
        //         generatePotentialMatchCoordinates(newPixel, valueToPixelLocationsMap);

        //     cout << "For new pixel: " << x << ", " << y << " : " << int(pixelVal) << " There were
        //     "
        //          << potentialMatchCoordinates.size() << " potential match locations:" << endl;
        //     for (auto pmCoords : potentialMatchCoordinates) {
        //         cout << "\t" << pmCoords.first << ", " << pmCoords.second << endl;
        //     }

        //     for (auto pmCoords : potentialMatchCoordinates) {
        //         bool mbr = matchesAtByRotation(reactionRule, rm, pmCoords);
        //         auto [i, j] = pmCoords;

        //         if (mbr) {
        //             uniqueMatchLocations.emplace(pmCoords);
        //             cout << "Using matchesAtByRotation() there is a match at " << i << ", " << j
        //                  << " for " << rm->angle_ << " degrees rotatation." << endl;
        //         }
        //     }
        // }
        // for (auto umc : uniqueMatchLocations) {
        //     auto [ux, uy] = umc;
        //     cout << "For rotation " << rm->angle_ << " there is a unique match at: " << ux << ",
        //     "
        //          << uy << endl;
        //     laydownMatchPixels2(reactionRule, umc, rm);
        // }
    }

    /**
     * @brief Apply replacement rules to main grid one time.
     *
     */
    void iterate(const std::string &s) {
        this->update();
        // this->beakerNode_->nodelog("ITERATING...");
        this->iterationCount_++;
        for (auto reactionRule : reactionRules_) {
            if (reactionRule->successor_ == reactionRule) {
                cout << "Rule: " << reactionRule->name_ << " has itself for successor." << endl;
                continue;
            }

            multiMatch(*reactionRule, r0__);
            multiMatch(*reactionRule, r90__);
            multiMatch(*reactionRule, r180__);
            multiMatch(*reactionRule, r270__);
        }

        // this->beakerNode_->refresh();

        for (const auto &[key, value] : this->successionMap_) {
            auto [px, py] = key;
            std::vector<valuePriorityPairT> vpStack = value;
            cout << "coordinate: " << px << ", " << py << endl;
            for (auto [val, pri] : vpStack) {
                cout << "\tval = " << int(val) << ", pri = " << pri << endl;
            }
        }
        updateGrid();
        // this->beakerNode_->beakerCanvas_->drawGrid();
    }

    static void makeNewReactionRule_st(Beaker *b) { b->makeNewReactionRule(); }

   protected:
    shared_ptr<cl2::SignalBuilder> signalBuilder_;
    shared_ptr<GridControl<V>> gridControl_;
    shared_ptr<WebElementSignal<std::string>> nameInput_;
    shared_ptr<ObjectSignalLoop<std::string, Beaker<unsigned char>>> nameInputLoop_;
    // shared_ptr<WebElementSignal<std::string>> newRuleButtonOutput_;
    shared_ptr<WebElementSignal<std::string>> newRuleButton_;
    shared_ptr<WebElementSignal<std::string>> iterateButton_;
    shared_ptr<ObjectAcceptor<std::string, Beaker<V>>> objAcceptor_;
    shared_ptr<ObjectAcceptor<std::string, Beaker<V>>> iterateAcceptor_;
    std::string name_;
    bool isReactionRule_ = false;  //!< Set to true if this Beaker is being used as a reaction rule
                                   //!< for an enclosing Beaker.

    int gridPixelWidth_ = 600;   //!< Width in pixels of beaker grid.
    int gridPixelHeight_ = 400;  //!< Height in pixels of beaker grid.

    int gridWidth_ = 60;   //!< Width of beaker grid in cells.
    int gridHeight_ = 40;  //!< Height of beaker grid in cells.

    int ruleGridWidth_ = 5;   //!< Width of new rule grid in cells.
    int ruleGridHeight_ = 3;  //!< Height of new rule grid in cells.

    // V *gridArray_;  //!< The actual grid data to be used by the CanvasGrid in BeakerNode.

    int playMode_ = 0;

    std::map<gridCoordinatePairT, std::vector<valuePriorityPairT>> successionMap_;

    int iterationCount_ = 0;
    //!< Counter that advances every time the rules are applied to the grid.

    std::vector<shared_ptr<Beaker<V>>> reactionRules_;

    Beaker *parentBeaker_;

    shared_ptr<Beaker<V>> successor_;  // = this;    //!< The pattern we replace this one with.
    int successorOffsetX_ = 0;         //!< X offset of replacement pattern.
    int successorOffsetY_ = 0;         //!< Y offset of replacement pattern.
    int successionPriority_ = 1;       //!< Priority assigned to pixels replaced by application of
                                       //!< this pattern. Lower values take precedence.

    int ruleCount_ = 0;

    bool clean_ = true;  //!< We set this to false when the user modifies a rule or the main grid.

    typedef std::map<V, std::vector<gridCoordinatePairT>> valueToCoordinateMapT;

    std::map<int, valueToCoordinateMapT> rotationToPixelListsMap_;
    std::map<int, std::vector<std::pair<V, std::vector<gridCoordinatePairT>>>>
        rotationToSortedPixelListsMap_;
};

EMSCRIPTEN_BINDINGS(PixelReactor) {
    emscripten::class_<Beaker<unsigned char>>("Beaker")
        .function("toggleClean", &Beaker<unsigned char>::toggleClean,
                  emscripten::allow_raw_pointers())
        .function("clearGrid", &Beaker<unsigned char>::clearGrid, emscripten::allow_raw_pointers())
        .function("makeDirty", &Beaker<unsigned char>::makeDirty, emscripten::allow_raw_pointers())
        .function("iterate", &Beaker<unsigned char>::iterate, emscripten::allow_raw_pointers())
        .function("makePixelList", &Beaker<unsigned char>::makePixelList,
                  emscripten::allow_raw_pointers())
        .function("makeNewReactionRule", &Beaker<unsigned char>::makeNewReactionRule,
                  emscripten::allow_raw_pointers());

    emscripten::class_<SignalBuilder>("SignalBuilder");
    emscripten::class_<GridControl<int>>("GridControl");

    emscripten::register_vector<Beaker<unsigned char>::gridCoordinatesValueTripletT>(
        "std::vector<gridCoordinatesValueTripletT>");
}

/**
 * @brief The Pixelreactor app from the old ClojureScript site redone in C++.
 *
 */
struct PixelReactor {
    int *ruleFrameWidth = new int(5);
    int *ruleFrameHeight = new int(3);
    shared_ptr<Beaker<unsigned char>> mainBeaker_;
    shared_ptr<SignalBuilder> signalBuilder_;
    shared_ptr<WebElementSignal<std::string>> ruleWidthInput_;
    shared_ptr<WebElementSignal<std::string>> ruleHeightInput_;
    // shared_ptr<ObjectEmitter<std::string, Beaker<unsigned char>>> objectEmitter_;
    // shared_ptr<ObjectAcceptor<std::string, Beaker<unsigned char>>> objectAcceptor_;
    shared_ptr<ObjectSignalLoop<std::string, Beaker<unsigned char>>> ruleWidthLoop_;
    shared_ptr<ObjectSignalLoop<std::string, Beaker<unsigned char>>> ruleHeightLoop_;

    PixelReactor(int defaultRuleframeWidth = 5, int defaultRuleframeHeight = 3) {
        cout << "I'm a Pixelreactor. I need to be redone completely 2!" << endl;
        signalBuilder_ = make_shared<cl2::SignalBuilder>();
        mainBeaker_ =
            make_shared<Beaker<unsigned char>>(signalBuilder_, 60, 40, 600, 400, "Beaker");
        mainBeaker_->finalize();

        // mainBeaker_->finalize();
        ruleWidthInput_ =
            signalBuilder_->withAttributes({{"class", val("small_width")}})
                .textInputWSS<std::string>("ruleWidthInput", "Rule Width in pixels", false);
        ruleHeightInput_ =
            signalBuilder_->withAttributes({{"class", val("small_width")}})
                .textInputWSS<std::string>("ruleHeightInput", "Rule Height in pixels", false);

        ruleWidthLoop_ = make_shared<ObjectSignalLoop<std::string, Beaker<unsigned char>>>(
            mainBeaker_, ruleWidthInput_, &Beaker<unsigned char>::setRuleGridWidth,
            &Beaker<unsigned char>::getRuleGridWidth);

        ruleHeightLoop_ = make_shared<ObjectSignalLoop<std::string, Beaker<unsigned char>>>(
            mainBeaker_, ruleHeightInput_, &Beaker<unsigned char>::setRuleGridHeight,
            &Beaker<unsigned char>::getRuleGridHeight);

        signalBuilder_->connectLoop(ruleWidthLoop_);
        signalBuilder_->connectLoop(ruleHeightLoop_);
    }
};

#endif
