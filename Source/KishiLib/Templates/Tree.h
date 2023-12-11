// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Array.h"

/**
 *
 */
template <typename ElementType>
class TTree
{

	typedef bool (*CompareFunction)(const TTree &tree1, const TTree &tree2);

public:
	TTree *parent = NULL;
	int layer = 0;
	TArray<TTree<ElementType> *> children;
	ElementType element;
	TTree() = default;
	TTree(const TTree &other) : element(other.element), children(other.children){};
	TTree(ElementType &element) : element(element){};
	TTree(ElementType &element, TArray<TTree *> &children) : element(element), children(children){};
	~TTree() = default;

	TTree &operator&=(const TTree &otherTree)
	{
		this->element = otherTree.element;
		this->parent = NULL;
		this->layer = 0;
		this->children = otherTree.children;
	};
	TTree &operator+=(const TTree &otherTree)
	{
		AddBranch(otherTree);
		return *this;
	};
	bool IsRoot()
	{
		return (parent == NULL);
	}
	bool IsLeaf()
	{
		return (children.Num() == 0);
	}

	TTree &AddBranch(const TTree &branch)
	{
		TTree newBranch = children.Add_GetRef(branch);
		BindChild(newBranch);
		return newBranch;
	};

	TTree &EmplaceAt(int index, const TTree &tree)
	{
		TTree newBranch = children.EmplaceAt_GetRef(index, tree);
		BindChild(newBranch);
		return newBranch;
	};

	TTree &GetChild(const TArray<int> &path)
	{
		TTree &current = *this;
		for (int i = 0; i < path.Num(); ++i)
		{
			current = current.children[i];
		}
	}
	bool Find(const TTree &tree, CompareFunction compare, TArray<int> &path)
	{
		if (Find_Rec(tree, compare, path))
		{
			SwapAll(path);
			return true;
		}
		return false;
	}

private:
	void BindChild(TTree &childBranch)
	{
		childBranch.parent = this;
		childBranch.layer = this->layer + 1;
	}
	void BindChild(int index)
	{
		BindChild(children[index]);
	}
	void BindAllChildren()
	{
		for (int i = 0; i < children.Num(); i++)
		{
			BindChild(i);
			children[i].BindAllchildren();
		}
	}
	bool Find_Rec(const TTree &tree, CompareFunction compare, TArray<int> &path)
	{
		if (compare(*this, tree))
		{
			return true;
		}
		if (IsLeaf())
		{
			return false;
		}
		for (int i = 0; i < children.Num(); i++)
		{
			if (children[i].Find_Rec(tree, compare, path))
			{
				path.Add(i);
				return true;
			}
		}
		return false;
	};
};
