#include "esp_err.h"
#include "esp_log.h"

#include "commands/CommandRequester.h"

CommandRequester::CommandRequester(const std::shared_ptr<const Node> &iNode, bool isBroadcast)
    : CommandRequester(iNode, isBroadcast, 0)
{
}

CommandRequester::CommandRequester(const std::shared_ptr<const Node> &iNode, bool isBroadcast, int iRequestId)
    : mIsBroadcast(isBroadcast),
      mNode(iNode),
      mRequestId(iRequestId),
      mResult()
{
}

bool CommandRequester::operator==(const CommandRequester &rhs)
{
  return (mIsBroadcast == rhs.mIsBroadcast) && (mNode.get() == rhs.mNode.get());
}

// Public methods

bool CommandRequester::isBroadcast() const
{
  return mIsBroadcast;
}

void CommandRequester::setBroadcast(bool iIsBroadcast)
{
  mIsBroadcast = iIsBroadcast;
}

const std::shared_ptr<const Node> &CommandRequester::getNode() const
{
  return mNode;
}

void CommandRequester::setNode(const std::shared_ptr<const Node> &iNode)
{
  mNode = iNode;
}

int CommandRequester::getId() const
{
  return mRequestId;
}

CommandResult &CommandRequester::getResult()
{
  return mResult;
}

const CommandResult &CommandRequester::getResult() const
{
  return mResult;
}

void CommandRequester::setResult(const CommandResult &iResult)
{
  mResult = iResult;
}
