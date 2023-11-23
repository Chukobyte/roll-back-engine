from typing import Optional

import crescent_internal
from crescent import Node, SceneTree, Node2D, NodeType


class TestCase:
    def __init__(self, name: str) -> None:
        self.name = name

    def __enter__(self) -> "TestCase":
        self._test_header()
        return self

    def __exit__(self, *args):
        self._test_footer()

    def _test_header(self):
        print(f"--------------- {self.name} ---------------")

    def _test_footer(self):
        print("---------------------------------------")


def are_floats_equal(a: float, b: float, rel_tol=0.00000001, abs_tol=0.0) -> bool:
    return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)


class TestNode(Node):
    def _start(self) -> None:
        print(f"Called start on {self}")

    @staticmethod
    def new() -> "TestNode":
        return crescent_internal.node_new("crescent_api_test", "TestNode", NodeType.Node)


# Tests
with TestCase("Node Tests") as test_case:
    # Create node
    new_node = Node.new()
    assert new_node
    scene_root = SceneTree.get_root()
    assert scene_root
    scene_root.add_child(new_node)
    # Node name
    node_name = new_node.name
    assert node_name
    # Add Child
    assert len(new_node.get_children()) == 0
    new_node_child1 = Node.new()
    new_node.add_child(new_node_child1)
    # Get Child
    assert len(new_node.get_children()) == 1
    # Time Dilation
    new_node.set_time_dilation(2.0)
    assert are_floats_equal(new_node.get_time_dilation(), 2.0)
    new_node.time_dilation = 1.0
    assert are_floats_equal(new_node.time_dilation, 1.0)
    # Test Custom Node
    test_node = TestNode.new()
    new_node.add_child(test_node)

    crescent_internal._scene_manager_process_queued_creation_entities()

    new_node.queue_deletion()
    assert new_node.is_queued_for_deletion()

with TestCase("Node2D Tests") as test_case:
    node2d = Node2D.new()
    print(f"node2d = {node2d}")

with TestCase("Scene Tree Tests") as test_case:
    SceneTree.change_scene("test_scene1.cscn")
    # TODO: Fix once scene manager updates are in
    # assert SceneTree.get_root()
