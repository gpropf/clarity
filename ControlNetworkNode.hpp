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

    // template <typename T>
    // class ModelNode;

    class ControlNetworkNode
    {

    public:
        /**
         * @brief Represents the 'edges' in our control graph. These edges can be active and contain a JS value that can act as a transformation
         * on the values moving between nodes.
         *
         *
         */

        class ActiveLink
        {
        public:
            ActiveLink(ControlNetworkNode *peer, val scalarConst = val(1));

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

        inline void setParent(ControlNetworkNode *parent) { this->parent_ = parent; }
        inline ControlNetworkNode *getParent() const { return this->parent_; }

        inline virtual val getVal() const
        {
            printNodeStats("getVal()");
            return val(NULL);
        }

        inline val getCLE() const { return cle_; }

        inline virtual void printState() const { cle_.call<void>("printState"); }
        inline static ControlNetworkNode *getCLElementById(const int id) { return switchboard[id]; }

        inline virtual string nodeStats() const
        {
            string s = "Node name: " + name_ + ", Node id: " + to_string(id_) + ", Node type: " + typeid(*this).name() + "\n";
            return s;
        }

        inline virtual void printNodeStats(const string &prefix = "") const
        {        
        }

        inline static void markNodeDirtyById(int id) { switchboard[id]->clean_ = false; }

        inline static void pushValToPeersThruALById(int id)
        {
            ControlNetworkNode *cnn = getCLElementById(id);
            cnn->printNodeStats("pushValToPeersThruALById()");
            cnn->pushValToPeersThruAL(cnn);
        }

        inline void toggleClean()
        {            
            clean_ = !clean_;
        }

        inline void init()
        {
            id_ = tm.getNext();
            ControlNetworkNode::switchboard[id_] = this;            
        }

        inline ControlNetworkNode()
        {
            init();
        }

        inline ControlNetworkNode(const string &name, const CppType storedValueType)
            : name_(name),
              storedValueType_(storedValueType)
        {
            init();            
            cle_.set("cpptype", val(storedValueType));
        }

        inline ControlNetworkNode(const CppType storedValueType) : storedValueType_(storedValueType)
        {
            init();            
            cle_.set("cpptype", val(storedValueType));
        }

        template <typename T>
        inline static val cpp2js(void *valptr)
        {
            return val(*reinterpret_cast<T *>(valptr));
        }        

        inline virtual void setVal(const val &inval)
        {        
            clean_ = false;
        }

        inline virtual void pushValToPeerThruAL(ActiveLink &al)
        {
            printNodeStats("pushValToPeerThruAL()");
            if (clean_)
            {         
            }

            val internalVal = getVal();

            if (internalVal.isNumber())
            {             
                val product = al.CLElement_.call<val>("applyTransformFn", al.transformFn_, internalVal);
                al.peer_->setVal(product);
            }
            else
            {
                al.peer_->setVal(internalVal);
            }

            clean_ = true;
        }        

        inline virtual void pushValToPeersThruAL(ControlNetworkNode *excludedPeer = nullptr)
        {
            printNodeStats("pushValToPeersThruAL()");
            if (excludedPeer == nullptr)
            {                
                for (auto alpeer : alpeers_)
                {             
                    pushValToPeerThruAL(alpeer);
                }
            }
            else
            {                
                for (auto alpeer : alpeers_)
                {
                    if (alpeer.peer_ != excludedPeer)
                    {
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
            al.peer_->addALPeer(ActiveLink(this, cle_.call<val>("invertValue", al.scalarConst_)), true);
        }

    protected:
        
        bool clean_ = true;
        static TicketMachine tm;
        static map<const int, ControlNetworkNode *> switchboard;
        val cle_ = val::global("CLElement").new_(); //!< An instance of the CLElement class that acts as a "proxy" in JS space
        
        CppType storedValueType_; //!< C++ Data type
        ControlNetworkNode *parent_;
        int id_ = 1000;
        string name_;
        vector<ControlNetworkNode::ActiveLink> alpeers_;
    };
}
#endif
