#pragma once

#include <optional>
#include <functional>
#include <memory>

#include "link/node/Node.h"
#include "commands/CommandResult.h"

class CommandRequester
{
public:
  // Constructors, copiers, and destructor

  CommandRequester(const std::shared_ptr<const Node> &iNode, bool iIsBroadcast);

  CommandRequester(const std::shared_ptr<const Node> &iNode, bool iIsBroadcast, int iRequestId);

  CommandRequester() = delete;

  ~CommandRequester() = default;

  CommandRequester(const CommandRequester &rhs) = default;

  CommandRequester &operator=(const CommandRequester &rhs) = default;

  bool operator==(const CommandRequester &rhs);

  // Public methods

  bool isBroadcast() const;

  void setBroadcast(bool iIsBroadcast);

  const std::shared_ptr<const Node> &getNode() const;

  void setNode(const std::shared_ptr<const Node> & iNode);

  int getId() const;

  CommandResult &getResult();

  const CommandResult &getResult() const;

  void setResult(const CommandResult &iResult);

private:
  // private class members

  bool mIsBroadcast;
  std::shared_ptr<const Node> mNode;
  int mRequestId;
  CommandResult mResult;
};
