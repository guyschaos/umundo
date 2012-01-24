#ifndef NODEQUERY_H_3YGLQKC0
#define NODEQUERY_H_3YGLQKC0

#include "common/stdInc.h"

#include <boost/enable_shared_from_this.hpp>

#include "thread/Thread.h"
#include "common/NodeStub.h"
#include "common/ResultSet.h"

namespace umundo {
	
	class NodeQuery : public boost::enable_shared_from_this<NodeQuery> {
	public:
		NodeQuery(string domain, ResultSet<NodeStub>*);
		virtual ~NodeQuery();
		
		virtual void added(shared_ptr<NodeStub>);
		virtual void changed(shared_ptr<NodeStub>);
		virtual void removed(shared_ptr<NodeStub>);
		virtual const string& getDomain();
		virtual void setTransport(string);
		virtual const string& getTransport();

		virtual void notifyImmediately(bool notifyImmediately);
		virtual void notifyResultSet();

	protected:
		bool _notifyImmediately;
		string _domain;
		string _transport;
		ResultSet<NodeStub>* _listener;

		Mutex _mutex;

		set<shared_ptr<NodeStub> > _pendingChanges;
		set<shared_ptr<NodeStub> > _pendingRemovals;
		set<shared_ptr<NodeStub> > _pendingAdditions;

		friend class DiscoveryImpl;
	};
}

#endif /* end of include guard: NODEQUERY_H_3YGLQKC0 */
