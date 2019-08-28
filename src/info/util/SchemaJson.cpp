#include "info/util/SchemaJson.h"
#include "cinder/Json.h"

using namespace info;
using namespace info::util;

ci::JsonTree getJsonTree(const Schema::Instance& inst){
  ci::JsonTree tree;

  tree.addChild(ci::JsonTree("id", inst.id));
  tree.addChild(ci::JsonTree("type", inst.typeId));
  tree.addChild(ci::JsonTree("name", inst.name));

  return tree;
}

ci::JsonTree getJsonTree(const Schema::Connection& conn){
  ci::JsonTree tree;

  tree.addChild(ci::JsonTree("id", conn.id));
  tree.addChild(ci::JsonTree("sender_instance", conn.output.instanceId));
  tree.addChild(ci::JsonTree("sender_port", conn.output.portId));
  tree.addChild(ci::JsonTree("sender_order", conn.output.order));
  tree.addChild(ci::JsonTree("receiver_instance", conn.input.instanceId));
  tree.addChild(ci::JsonTree("receiver_port", conn.input.portId));

  return tree;
}

ci::JsonTree getJsonTree(const Schema::InitialValue& val){
  ci::JsonTree tree;

  tree.addChild(ci::JsonTree("id", val.id));
  tree.addChild(ci::JsonTree("instance", val.instanceId));
  tree.addChild(ci::JsonTree("port", val.portId));
  tree.addChild(ci::JsonTree("type", val.typeName));
  tree.addChild(ci::JsonTree("value", "<TODO>"));


  return tree;
}

ci::JsonTree getJsonTree(const Schema::Implementation& impl){
  ci::JsonTree tree;

  {
    ci::JsonTree subtree = ci::JsonTree::makeArray("instances");

    for(auto inst : impl.instances) {
      auto insttree = getJsonTree(*inst);
      subtree.addChild(insttree);
    }

    tree.addChild(subtree);
  }

  {
    ci::JsonTree subtree = ci::JsonTree::makeArray("connections");

    for(auto conn : impl.connections) {
      auto insttree = getJsonTree(*conn);
      subtree.addChild(insttree);
    }

    tree.addChild(subtree);
  }
  
  {
    ci::JsonTree subtree = ci::JsonTree::makeArray("initial_values");

    for(auto conn : impl.initialValues) {
      auto insttree = getJsonTree(*conn);
      subtree.addChild(insttree);
    }

    tree.addChild(subtree);
  }
  
  return tree;
}

ci::JsonTree getJsonTree(const Schema& schema){
    ci::JsonTree tree;
    // this->collection->each([this, &collectionTree](shared_ptr<ItemType> itemRef){
    //     collectionTree.addChild(this->getItemJsonTree(itemRef));
    // })
    ci::JsonTree implsTree = ci::JsonTree::makeArray("implementations");
    
    for(auto implementationRef : schema.getImplementations()) {
      implsTree.addChild(getJsonTree(*implementationRef));
    }

    tree.addChild(implsTree);

    return tree;
}


std::string SchemaJson::serialize(const Schema& schema) {
  return getJsonTree(schema).serialize();
}

void SchemaJson::write_serialized(const Schema& schema, const ci::fs::path& path) {
    auto targetPathRef = ci::writeFile(path);
    targetPathRef->getStream()->write(serialize(schema));
}