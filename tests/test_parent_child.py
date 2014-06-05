"""Tests the parent-child relationships."""
from make_cffi import *
from .conf import *

def parent_child():
	"""This function does a bunch of useful setup as well as being a test; thus, we return all the objects."""
	graph = ffi.new("LavObject **")
	node1, node2 = ffi.new("LavObject **"), ffi.new("LavObject **")
	lav.Lav_createGraph(44100, 128, graph)
	lav.Lav_createNode(0, 1, lav.Lav_NODETYPE_ZEROS, graph[0], node1)
	lav.Lav_createNode(1, 0, lav.Lav_NODETYPE_ZEROS, graph[0], node2)
	node1, node2 = node1[0], node2[0]
	assert node1 is not ffi.NULL and node2 is not ffi.NULL
	#the following let us pull out info.
	parent = ffi.new("LavObject **")
	slot = ffi.new("unsigned int*")
	assert lav.Lav_setParent(node2, node1, 0, 0) == lav.Lav_ERROR_NONE
	assert lav.Lav_getParent(node2, 0, parent, slot) == lav.Lav_ERROR_NONE
	assert parent[0] == node1 and slot[0] == 0
	return graph[0], node1, node2, parent, slot

def test_parent_child():
	assert lav.Lav_initializeLibrary() == lav.Lav_ERROR_NONE
	parent_child()

def test_clear_parent():
	assert lav.Lav_initializeLibrary() == lav.Lav_ERROR_NONE
	graph, node1, node2, parent, slot = parent_child()
	lav.Lav_clearParent(node2, 0)
	lav.Lav_getParent(node2, 0, parent, slot)
	assert parent[0] == ffi.NULL and slot[0] == 0
