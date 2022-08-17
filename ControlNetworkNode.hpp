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
         * @brief Represents the 'edges' in our control graph. These edges can be active and contain a JS value that can act as a transformation
         * on the values moving between nodes.
         *
         *
         */

        class ActiveLink //: public Invertable
        {
        public:
              

            ActiveLink(ControlNetworkNode *peer, val scalarConst = val(1))
                : peer_(peer), scalarConst_(scalarConst)

            {
                CLElement_ = val::global("CLElement");
                transformFn_ = CLElement_.call<val>("generateTransformFn", scalarConst_);
            }

            template <typename T>
            ActiveLink(ControlNetworkNode *peer, const T scalarConst)
                : peer_(peer), scalarConst_(val(scalarConst))

            {
                CLElement_ = val::global("CLElement");
                transformFn_ = CLElement_.call<val>("generateTransformFn", scalarConst_);
            }

            ControlNetworkNode *peer_;
            val scalarConst_;
            val CLElement_;
            val transformFn_;
        };

        void setParent(ControlNetworkNode *parent) { this->parent_ = parent; }
        ControlNetworkNode *getParent() const { return this->parent_; }

        virtual val getVal() const
        {
            printNodeStats("getVal()");
            return val(NULL);
        }

        val getJSval() const { return jsval_; }

        virtual void printState() const { jsval_.call<void>("printState"); }
        static ControlNetworkNode *getCLElementById(const int id) { return switchboard[id]; }

        virtual string nodeStats() const
        {
            string s = "Node name: " + name_ + ", Node id: " + to_string(id_) + ", Node type: " + typeid(*this).name() + "\n";
            return s;
        }

        virtual void printNodeStats(const string &prefix = "") const
        {
            // cout<< prefix << " " << nodeStats();
        }

        static void markNodeDirtyById(int id) { switchboard[id]->clean_ = false; }

        static void pushValToPeersThruALById(int id)
        {
            ControlNetworkNode *cnn = getCLElementById(id);
            cnn->printNodeStats("pushValToPeersThruALById()");
            cnn->pushValToPeersThruAL(cnn);
        }

        void toggleClean()
        {
            // cout<< "TOGGLECLEAN, oldstate: " << clean_ << " ID = " << id_ << " \n\n\n";
            clean_ = !clean_;
        }

        void init()
        {
            id_ = tm.getNext();
            ControlNetworkNode::switchboard[id_] = this;
            // cout<< "ControlNetworkNode(): Setting id to " << id_ << "\n";
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
            // cout<< "ControlNetworkNode(const string &name, const CppType anyvalPtrType): "
            //     << (int)anyvalPtrType << " ID = " << id_ << " \n";
            jsval_.set("cpptype", val(anyvalPtrType));
        }

        ControlNetworkNode(const CppType anyvalPtrType) : anyvalPtrType_(anyvalPtrType)
        {
            init();
            // cout<< "ControlNetworkNode(const CppType anyvalPtrType): " << (int)anyvalPtrType << " ID = " << id_ << " \n";
            jsval_.set("cpptype", val(anyvalPtrType));
        }

        template <typename T>
        inline static val cpp2js(void *valptr)
        {
            return val(*reinterpret_cast<T *>(valptr));
        }
        
        virtual void setVal(const val &inval)
        {
            // cout<< "Marking node " << id_ << " as dirty.\n\n";
            clean_ = false;
        }

        virtual void pushValToPeerThruAL(ActiveLink &al)
        {
            printNodeStats("pushValToPeerThruAL()");
            if (clean_)
            {
                // cout<< "pushValToPeerThruAL: Node " << id_ << " is clean.\n";
            }

            val internalVal = getVal();

            if (internalVal.isNumber())
            {
                //val CLElement_ = val::global("CLElement");                
                //val product = jsval_.call<val>("multiplyValues", internalVal, al.scalarConst_);
                val product = al.CLElement_.call<val>("applyTransformFn", al.transformFn_, internalVal);
                al.peer_->setVal(product);
            }
            else
            {
                al.peer_->setVal(internalVal);
            }

            clean_ = true;
        }

        void printALPeers(string prefix = "")
        {
            for (auto alpeer : alpeers_)
            {
                // cout<< prefix << " printALPeers: id = " << id_ << ", ALPeer id = " << alpeer.peer_->id_ << "\n";
            }
        }

        // void setValOnALPeers(const val &inval)
        // {
        //     for (auto alpeer : alpeers_)
        //     {
        //         val product = jsval_.call<val>("multiplyValues", inval, alpeer.scalarConst_);
        //         alpeer.peer_->setVal(product);
        //         // alpeer.peer_->setVal(inval);
        //     }
        // }

        virtual void pushValToPeersThruAL(ControlNetworkNode *excludedPeer = nullptr)
        {
            printNodeStats("pushValToPeersThruAL()");
            if (excludedPeer == nullptr)
            {
                // cout<< "pushValToPeersThruAL(): excludedPeer == nullptr\n";
                for (auto alpeer : alpeers_)
                {
                    // cout<< "pushValToPeersThruAL(): alpeer.peer_.id_ = " << alpeer.peer_->id_ << "\n";
                    pushValToPeerThruAL(alpeer);
                }
            }
            else
            {
                // cout<< "pushValToPeersThruAL(): excludedPeer != nullptr\n";
                for (auto alpeer : alpeers_)
                {
                    if (alpeer.peer_ != excludedPeer)
                    {
                        // cout<< "pushValToPeersThruAL(): alpeer.peer_ != excludedPeer\n";

                        pushValToPeerThruAL(alpeer);
                    }
                }
            }
        }

        void
        addALPeer(ControlNetworkNode::ActiveLink al, bool alreadyAdded = false)
        {
            alpeers_.push_back(al);
            if (alreadyAdded)
            {
                return;
            }
            al.peer_->addALPeer(ActiveLink(this, jsval_.call<val>("invertValue", al.scalarConst_)), true);
        }

    protected:
        bool clean_ = true;
        static TicketMachine tm;
        static map<const int, ControlNetworkNode *> switchboard;
        val jsval_ = val::global("CLElement").new_();
        CppType anyvalPtrType_; // C++ Data type
        ControlNetworkNode *parent_;
        int id_ = 1000;
        string name_;
        vector<ControlNetworkNode::ActiveLink> alpeers_;
    };
}
#endif
