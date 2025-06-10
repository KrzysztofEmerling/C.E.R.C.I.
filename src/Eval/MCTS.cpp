#include "MCTS.h"

MCTS_node::MCTS_node(MCTS_node *parent, Move move)
    : m_Parent(parent), m_MoveFromParent(move), m_ChildrenCount(0),
      m_VisitCount(0), m_EvalSum(0)
{
}

MCTS_node::~MCTS_node()
{
    for (int i = 0; i < m_ChildrenCount; ++i)
        delete m_Children[i];
}

MCTS_node *MCTS_node::AddChild(const Move &move)
{
    MCTS_node *child = new MCTS_node(this, move);
    if (m_ChildrenCount < 218)
        m_Children[m_ChildrenCount++] = child;
    return child;
}

void MCTS_node::UpdateStats(int eval)
{
    ++m_VisitCount;
    m_EvalSum += eval;
}

MCTS_node *MCTS_node::GetBestChild() const
{
    MCTS_node *best = nullptr;
    double bestScore = -1.0;
    for (int i = 0; i < m_ChildrenCount; ++i)
    {
        double score = m_Children[i]->GetEvalAverage();
        if (score > bestScore)
        {
            bestScore = score;
            best = m_Children[i];
        }
    }
    return best;
}

double MCTS_node::GetUCB(double parentVisits, double exploration) const
{
    if (m_VisitCount == 0)
        return std::numeric_limits<double>::infinity();
    double avg = GetEvalAverage();
    return avg + exploration * std::sqrt(std::log(parentVisits) / m_VisitCount);
}