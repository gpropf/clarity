#ifndef ControlNetworkNode_hpp
#define ControlNetworkNode_hpp

#include "clarity.hpp"

namespace clarity
{

    /**
     * @brief The project's central class. Describes an element with push/pull behavior to syncronize a data model and
     * a web view. WebElements can contain others and complex web controls can be built up in a hierarchal fashion. The class is
     * not necessarily homologous to JavaScript Dom elements but the "mirror" class CLElement is currently used to store
     * a JS Dom element and retain other state on the JS side. This is also the base class for all other types.
     * The CNN type should never be instantiated as it has no specific behaviors and doesn't bind to anything
     * on the webpage. In future it may contain pure virtual methods and trying to instantiate it will be an error.
     *
     */
    class ControlNetworkNode
    {

    public:
        /**
         * @brief Represents the 'edges' in our control graph. The union contains either a JavaScript
         * function or a multiplier. In practice most control elements will only need a multiplier value
         * to convert back and forth between some internal value in the model and the value displayed on
         * the web. A good example might be writing a car engine simulator where the internal physics are
         * all done in SI units but the user is more familiar with Imperial units. So computations can still
         * be done with Watts and meters but the user sees horsepower and feet.
         *
         *
         */

        class ActiveLink //: public Invertable
        {
        public:
            ActiveLink(ControlNetworkNode *peer, val multiplier = val(1))
                : peer_(peer), multiplier_(multiplier)

            {
            }

            ControlNetworkNode *peer_;
            val multiplier_;
        };

        void setParent(ControlNetworkNode *parent) { this->parent_ = parent; }
        ControlNetworkNode *getParent() const { return this->parent_; }

        virtual val getVal() const
        {
            cout << "ControlNetworkNode::getVal()"
                 << "\n";
            printNodeStats("getVal()");
            return val(NULL);
        }

        val getJSval() const { return jsval_; }

        // void splicePtrs(void *worldValuePtr) { anyvalPtr_ = worldValuePtr; }
        virtual void printState() const { jsval_.call<void>("printState"); }
        static ControlNetworkNode *getCLElementById(const int id) { return switchboard[id]; }

        virtual string nodeStats() const
        {
            string s = "Node id: " + to_string(id_) + ", Node type: " + typeid(*this).name() + "\n";
            return s;
        }

        virtual void printNodeStats(const string &prefix = "") const
        {
            cout << prefix << " " << nodeStats();
        }

        static void markNodeDirtyById(int id) { switchboard[id]->clean_ = false; }

        static void pushValToPeersById(int id)
        {
            ControlNetworkNode *cnn = getCLElementById(id);
            cnn->printNodeStats("pushValToPeersById()");
            cnn->pushValToPeers(cnn);
        }

        static void pushValToPeersThruALById(int id)
        {
            ControlNetworkNode *cnn = getCLElementById(id);
            cnn->printNodeStats("pushValToPeersThruALById()");
            cnn->pushValToPeersThruAL(cnn);
        }

        void toggleClean()
        {
            cout << "TOGGLECLEAN, oldstate: " << clean_ << " ID = " << id_ << " \n\n\n";
            clean_ = !clean_;
        }

        void init()
        {
            id_ = tm.getNext();
            ControlNetworkNode::switchboard[id_] = this;
            cout << "ControlNetworkNode(): Setting id to " << id_ << "\n";
        }

        ControlNetworkNode()
        {
            init();
        }

        ControlNetworkNode(const string &name, const CppType anyvalPtrType)
            : name_(name),
              anyvalPtrType_(anyvalPtrType)
        {
            init();
            // ControlNetworkNode();
            cout << "ControlNetworkNode(const string &name, const CppType anyvalPtrType): "
                 << (int)anyvalPtrType << " ID = " << id_ << " \n";
            jsval_.set("cpptype", val(anyvalPtrType));
        }

        // ControlNetworkNode(const DynamicValue dynamicValue, const string &name = "") : dynamicValue_(dynamicValue), name_(name)
        // {
        // }

        ControlNetworkNode(const CppType anyvalPtrType) : anyvalPtrType_(anyvalPtrType)
        {
            init();
            cout << "ControlNetworkNode(const CppType anyvalPtrType): " << (int)anyvalPtrType << " ID = " << id_ << " \n";
            jsval_.set("cpptype", val(anyvalPtrType));
        }

        template <typename T>
        inline static val cpp2js(void *valptr)
        {
            return val(*reinterpret_cast<T *>(valptr));
        }

        virtual void updatePeers() {}
        virtual void setVal(const val &inval)
        {
            cout << "Marking node " << id_ << " as dirty.\n\n";
            clean_ = false;
        }

        virtual void pushValToPeer(ControlNetworkNode *peer)
        {
            printNodeStats("pushValToPeer()");
            if (clean_)
            {
                cout << "Node " << id_ << " is clean, should return.\n";
                // return;
            }
            val internalVal = getVal();
            cout << "Internal val is ";
            jsval_.call<void>("printToConsole", internalVal);
            cout << "Value is type ";
            jsval_.call<void>("printType", internalVal);
            cout << endl;
            peer->setVal(internalVal);
            peer->pushValToPeers(this);
            clean_ = true;
        }

        // bool isType(emscripten::val value, const std::string &type)
        // {
        //     return (value.typeof().as<std::string>() == type);
        // }

        virtual void pushValToPeerThruAL(ActiveLink &al)
        {
            printNodeStats("pushValToPeerThruAL()");
            if (clean_)
            {
                cout << "pushValToPeerThruAL: Node " << id_ << " is clean.\n";
                // return;
            }

            val internalVal = getVal();

            if (internalVal.isNumber())
            {
                val product = jsval_.call<val>("multiplyValues", internalVal, al.multiplier_);
                al.peer_->setVal(product);
            }
            else {
                al.peer_->setVal(internalVal);
            }

            // cout << "PRODUCT: " << product.as<T>() << "\n";
            clean_ = true;
        }

        virtual void pushValToPeers(ControlNetworkNode *excludedPeer = nullptr)

        {
            printNodeStats("pushValToPeers()");
            if (excludedPeer == nullptr)
            {
                for (auto peer : peers_)
                {
                    pushValToPeer(peer);
                }
            }
            else
            {
                for (auto peer : peers_)
                {
                    if (peer != excludedPeer)
                    {
                        pushValToPeer(peer);
                    }
                }
            }
        }

        void printALPeers(string prefix = "")
        {
            for (auto alpeer : alpeers_)
            {
                cout << prefix << " printALPeers: id = " << id_ << ", ALPeer id = " << alpeer.peer_->id_ << "\n";
            }
        }

        void setValOnALPeers(const val &inval)
        {
            for (auto alpeer : alpeers_)
            {
                val product = jsval_.call<val>("multiplyValues", inval, alpeer.multiplier_);
                alpeer.peer_->setVal(product);
                // alpeer.peer_->setVal(inval);
            }
        }

        virtual void pushValToPeersThruAL(ControlNetworkNode *excludedPeer = nullptr)
        {
            printNodeStats("pushValToPeersThruAL()");
            if (excludedPeer == nullptr)
            {
                cout << "pushValToPeersThruAL(): excludedPeer == nullptr\n";
                for (auto alpeer : alpeers_)
                {
                    cout << "pushValToPeersThruAL(): alpeer.peer_.id_ = " << alpeer.peer_->id_ << "\n";
                    pushValToPeerThruAL(alpeer);
                }
            }
            else
            {
                cout << "pushValToPeersThruAL(): excludedPeer != nullptr\n";
                for (auto alpeer : alpeers_)
                {
                    if (alpeer.peer_ != excludedPeer)
                    {
                        cout << "pushValToPeersThruAL(): alpeer.peer_ != excludedPeer\n";

                        pushValToPeerThruAL(alpeer);
                    }
                }
            }
        }

        void
        addPeer(ControlNetworkNode *peer, bool alreadyAdded = false)
        {

            peers_.push_back(peer);
            if (alreadyAdded)
            {
                return;
            }
            peer->addPeer(this, true);
        }

        void
        addALPeer(ControlNetworkNode::ActiveLink al, bool alreadyAdded = false)
        {

            alpeers_.push_back(al);
            if (alreadyAdded)
            {
                return;
            }
            al.peer_->addALPeer(ActiveLink(this, jsval_.call<val>("invertValue", al.multiplier_)), true);
        }

    protected:
        bool clean_ = true;
        static TicketMachine tm;
        static map<const int, ControlNetworkNode *> switchboard;
        val jsval_ = val::global("CLElement").new_();
        // val foo = val::global("Foo").new_();
        CppType anyvalPtrType_; // C++ Data type
        // void *anyvalPtr_;       // pointer to actual data
        ControlNetworkNode *parent_;
        int id_ = 1000;
        string name_;
        vector<ControlNetworkNode *> peers_;
        vector<ControlNetworkNode::ActiveLink> alpeers_;
    };
}
#endif
