#include "BoardState.h"
#include <cmath>

class MCTS_node
{
private:
    MCTS_node *m_Parent;
    Move m_MoveFromParent;
    MCTS_node *m_Children[218];
    int m_ChildrenCount;

    int m_VisitCount;
    u64 m_EvalSum;

public:
    MCTS_node(MCTS_node *parent = nullptr, Move move = Move{});
    ~MCTS_node();

    inline bool IsLeaf() const { return m_ChildrenCount == 0; }
    inline MCTS_node *GetParent() const { return m_Parent; }
    inline const Move &GetMove() const { return m_MoveFromParent; }
    inline int GetVisitCount() const { return m_VisitCount; }
    inline double GetEvalAverage() const { return m_VisitCount > 0 ? static_cast<double>(m_EvalSum) / m_VisitCount : 0.0; }
    inline int GetChildrenCount() const { return m_ChildrenCount; }
    inline MCTS_node *GetChild(int i) const { return (i < m_ChildrenCount) ? m_Children[i] : nullptr; }

    MCTS_node *AddChild(const Move &move);

    void UpdateStats(int eval);

    MCTS_node *GetBestChild() const;

    double GetUCB(double parentVisits, double exploration = 14.15) const;
};
