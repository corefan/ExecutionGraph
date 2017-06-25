
#include "TestFunctions.hpp"

#include <meta/meta.hpp>
#include "ExecutionGraph/nodes/LogicNode.hpp"
#include "ExecutionGraph/nodes/LogicSocket.hpp"

using namespace executionGraph;

using Config = GeneralConfig<>;

template<typename TConfig>
class IntegerNode : public TConfig::NodeBaseType
{
public:
    using Config = TConfig;
    using Base   = typename Config::NodeBaseType;

    enum Ins
    {
        Value1,
        Value2
    };
    enum Outs
    {
        Result1,
    };
    EXEC_GRAPH_DEFINE_SOCKET_TRAITS(Ins, Outs);

    using InSockets = InSocketDeclList<InSocketDecl<Value1, int>,
                                       InSocketDecl<Value2, int>>;

    using OutSockets = OutSocketDeclList<OutSocketDecl<Result1, int>>;

    EXEC_GRAPH_DEFINE_LOGIC_NODE_GET_TYPENAME()
    EXEC_GRAPH_DEFINE_LOGIC_NODE_VALUE_GETTERS(Ins, InSockets, Outs, OutSockets)

    template<typename... Args>
    IntegerNode(Args&&... args)
        : Base(std::forward<Args>(args)...)
    {
        // Add all sockets
        this->template addSockets<InSockets>(std::make_tuple(2, 2));
        this->template addSockets<OutSockets>(std::make_tuple(0));
    }

    void reset() override{};

    void compute() override
    {
        // ugly syntax due to template shit
        //  this->template getValue<typename OutSockets::template Get<Result1>>() =
        //      this->template getValue<typename InSockets::template Get<Value1>>() +
        //      this->template getValue<typename InSockets::template Get<Value2>>();
        //
        // The macro: EXEC_GRAPH_DEFINE_LOGIC_NODE_VALUE_GETTERS defines some useful
        // nicer syntax!
        getOutVal<Result1>() = getInVal<Value1>() + getInVal<Value2>();
    }
};

MY_TEST(Node_Test, Int_Int)
{
    // Integer node connection (wrong connection)
    IntegerNode<Config> node1(1);
    IntegerNode<Config> node2(2);

    node1.addWriteLink(0, node2, 1);  // Correct connection!

    try
    {
        node1.addWriteLink(0, node2, 2);  // Wrong connection!
    }
    catch (NodeConnectionException& e)
    {
        std::cout << "Correct Exception: " << e.what() << std::endl;
        return;
    }

    throw std::runtime_error("Exception not catched!!!");
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
