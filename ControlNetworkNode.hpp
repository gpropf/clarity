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

        class ActiveLink
        {
        protected:
            void *multiplier;
            ActiveLink(ControlNetworkNode *peer, void *multiplier,
                       CppType cppType) : peer_(peer), multiplier_(multiplier), cppType_(cppType)

            {
            }
            ControlNetworkNode *peer_;
            void *multiplier_;
            CppType cppType_;

            // ActiveLink invert(ControlNetworkNode *owner, ActiveLink al)
            // {
            //     return ActiveLink(owner, 1 / al.getMultiplier(),
            //                       al.getJsFunc(), al.getValueTransformIsConstant());
            // }

        public:
            inline void *getMultiplier() { return multiplier_; }
            // inline bool getValueTransformIsConstant() { return valueTransformIsConstant_; }
            // inline val getJsFunc() { return vt_.jsFunc_; }
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

        void splicePtrs(void *worldValuePtr) { anyvalPtr_ = worldValuePtr; }
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

        void toggleClean()
        {
            cout << "TOGGLECLEAN, oldstate: " << clean_ << " ID = " << id_ << " \n\n\n";
            clean_ = !clean_;
        }

        ControlNetworkNode()
        {
            id_ = tm.getNext();
            ControlNetworkNode::switchboard[id_] = this;
            cout << "ControlNetworkNode(): Setting id to " << id_ << "\n";
        }

        // ControlNetworkNode(const string &name) : name_(name)
        // {
        //     ControlNetworkNode();
        //     if (name == "d-test")
        //     {
        //         cout << "DTEST:ControlNetworkNode(const string &name): id = " << id_ << "\n";
        //     }
        // }

        ControlNetworkNode(const string &name, const CppType anyvalPtrType)
            : name_(name),
              anyvalPtrType_(anyvalPtrType)
        {
            if (name == "d-test")
            {
                cout << "DTEST:Before " << (int)anyvalPtrType << " id = " << id_ << "\n";
            }
            ControlNetworkNode();
            if (name == "d-test")
            {
                cout << "DTEST:After " << (int)anyvalPtrType << " id = " << id_ << "\n";
            }
            cout << "ControlNetworkNodeControlNetworkNode\n\n\n";
            if (name == "d-test")
            {
                cout << "DTEST:ControlNetworkNode(const string &name, const CppType anyvalPtrType): " << (int)anyvalPtrType << " id = " << id_ << "\n";
            }
            jsval_.set("cpptype", val(anyvalPtrType));
        }

        ControlNetworkNode(ControlNetworkNode *parent) : parent_(parent)
        {
            ControlNetworkNode();
        }

        ControlNetworkNode(const CppType anyvalPtrType) : anyvalPtrType_(anyvalPtrType)
        {
            ControlNetworkNode();
            cout << "ControlNetworkNode(const CppType anyvalPtrType): " << (int)anyvalPtrType << "\n";
            id_ = tm.getNext();
            jsval_.set("cpptype", val(anyvalPtrType));
            ControlNetworkNode::switchboard[id_] = this;
        }

        ControlNetworkNode(void *anyvalPtr) : anyvalPtr_(anyvalPtr)
        {
            ControlNetworkNode();
            id_ = tm.getNext();
            ControlNetworkNode::switchboard[id_] = this;
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
            string name_;
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
            // al.peer->addALPeer()
        }

    protected:
        bool clean_ = true;
        static TicketMachine tm;
        static map<const int, ControlNetworkNode *> switchboard;
        val jsval_ = val::global("CLElement").new_();
        val nodeVal = val(NULL);
        CppType anyvalPtrType_; // C++ Data type
        void *anyvalPtr_;       // pointer to actual data
        ControlNetworkNode *parent_;
        int id_ = 7;
        string name_;
        vector<ControlNetworkNode *> peers_;
        vector<ControlNetworkNode::ActiveLink> alpeers_;
    };
}
#endif
