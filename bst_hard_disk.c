#include "assignment_4.h"

void add_freehead(FILE *fp, int curos);
void pr_inorder(int os, FILE *fp);
void pr_preorder(int os, FILE *fp);

FILE* init_tree(const char *filename)
{
	FILE *fp = fopen(filename, "r+");
	if (fp == NULL)
	{
		fp = fopen(filename, "w+");
		tree_t tree;
		tree.free_head = -1;
		tree.root = -1;
		fseek(fp, 0, SEEK_SET);
		fwrite(&tree, sizeof(tree_t), 1, fp);
	}
	return fp;
}

void close_tree(FILE *fp)
{
	fclose(fp);
}

void display_inorder(FILE *fp)
{
	tree_t tree;
	fseek(fp, 0, SEEK_SET);
	fread(&tree, sizeof(tree_t), 1, fp);
	if (tree.root == -1)
		return;
	pr_inorder(tree.root, fp);  //recursive function for printing inorder
	printf("\n");
}

void pr_inorder(int os, FILE *fp)
{
	node_t node;
	fseek(fp, os, SEEK_SET);
	fread(&node, sizeof(node_t), 1, fp);
	if (node.left_offset != -1)
		pr_inorder(node.left_offset, fp);
	printf("%d ", node.key);
	if (node.right_offset != -1)
		pr_inorder(node.right_offset, fp);
}

void display_preorder(FILE *fp)
{
	tree_t tree;
	fseek(fp, 0, SEEK_SET);
	fread(&tree, sizeof(tree_t), 1, fp);
	if (tree.root == -1)
		return;
	pr_preorder(tree.root, fp);   //recursive function for printing preorder
	printf("\n");
}

void pr_preorder(int os, FILE *fp)
{
	node_t node;
	fseek(fp, os, SEEK_SET);
	fread(&node, sizeof(node_t), 1, fp);
	printf("%d ", node.key);
	if (node.left_offset != -1)
		pr_inorder(node.left_offset, fp);
	if (node.right_offset != -1)
		pr_inorder(node.right_offset, fp);
}

void insert_key(int key, FILE *fp)
{
	tree_t tree;
	fseek(fp, 0, SEEK_SET);
	fread(&tree, sizeof(tree_t), 1, fp);
	node_t node;
	node.key = key;
	node.left_offset = -1;
	node.right_offset = -1;
	if (tree.root == -1)  //tree is empty
	{
		fseek(fp, sizeof(tree_t), SEEK_SET);
		tree.root = ftell(fp);
		fwrite(&node, sizeof(node_t), 1, fp);
		fseek(fp, 0, SEEK_SET);
		fwrite(&tree, sizeof(tree_t), 1, fp);
	}
	else
	{
		node_t fre;
		int flag = 0;
		int check = 1;
		int os;
		int curos = 8;
		if (tree.free_head == -1)	//checks for free space in the file to insert
		{
			fseek(fp, 0, SEEK_END);
			os = ftell(fp);
		}
		else
		{
			fseek(fp, tree.free_head, SEEK_SET);
			fread(&fre, sizeof(node_t), 1, fp);
			flag = 1;
			os = tree.free_head;
		}
		fseek(fp, tree.root, SEEK_SET);
		node_t cur;
		fread(&cur, sizeof(node_t), 1, fp);
		while (check)
		{
			if (cur.key == key)
			{
				return;
			}
			else if (cur.key > key)
			{
				if (cur.left_offset == -1)
				{
					cur.left_offset = os;
					fseek(fp, curos, SEEK_SET);
					fwrite(&cur, sizeof(node_t), 1, fp);
					fseek(fp, os, SEEK_SET);
					fwrite(&node, sizeof(node_t), 1, fp);
					check = 0;
				}
				else
				{
					fseek(fp, cur.left_offset, SEEK_SET);
					curos = ftell(fp);
					fread(&cur, sizeof(node_t), 1, fp);
				}
			}
			else if (cur.key < key)
			{
				if (cur.right_offset == -1)
				{
					cur.right_offset = os;
					fseek(fp, curos, SEEK_SET);
					fwrite(&cur, sizeof(node_t), 1, fp);
					fseek(fp, os, SEEK_SET);
					fwrite(&node, sizeof(node_t), 1, fp);
					check = 0;
				}
				else
				{
					fseek(fp, cur.right_offset, SEEK_SET);
					curos = ftell(fp);
					fread(&cur, sizeof(node_t), 1, fp);
				}
			}
		}
		if (flag)
		{
			tree.free_head = fre.left_offset;
			fseek(fp, 0, SEEK_SET);
			fwrite(&tree, sizeof(tree_t), 1, fp);
		}
	}
}

void delete_key(int key, FILE *fp)
{
	tree_t tree;
	fseek(fp, 0, SEEK_SET);
	fread(&tree, sizeof(tree_t), 1, fp);
	node_t cur;
	node_t pre;
	pre.key = -1;
	pre.left_offset = -1;
	pre.right_offset = -1;
	fseek(fp, tree.root, SEEK_SET);
	fread(&cur, sizeof(node_t), 1, fp);
	int found = -1;
	int prevos = -1;
	int curos = 8;
	int check = 1;
	while (check)
	{
		if (cur.key == key)
		{
			found = curos;
			check = 0;
		}
		else if (cur.key > key)
		{
			if (cur.left_offset == -1)
			{
				return;
			}
			else
			{
				pre = cur;
				prevos = curos;
				fseek(fp, cur.left_offset, SEEK_SET);
				curos = ftell(fp);
				fread(&cur, sizeof(node_t), 1, fp);
			}
		}
		else if (cur.key < key)
		{
			if (cur.right_offset == -1)
			{
				return;
			}
			else
			{
				pre = cur;
				prevos = curos;
				fseek(fp, cur.right_offset, SEEK_SET);
				curos = ftell(fp);
				fread(&cur, sizeof(node_t), 1, fp);
			}
		}
	}
	int c = -1;				//checks for cases
	if (found != -1)
	{
		if (cur.left_offset == -1 && cur.right_offset == -1)
		{
			c = 0;
		}
		else if (cur.left_offset == -1 || cur.right_offset == -1)
		{
			c = 1;
		}
		else
		{
			c = 2;
		}
	}
	switch (c)
	{
		case 0:
		case 1:
			{
				if (prevos != -1)
				{
					if (pre.left_offset == curos)
					{
						if (cur.left_offset != -1)
							pre.left_offset = cur.left_offset;
						else if (cur.right_offset != -1)
							pre.left_offset = cur.right_offset;
						else if (cur.right_offset == -1 && cur.left_offset == -1)
							pre.left_offset = -1;
					}
					else if (pre.right_offset == curos)
					{
						if (cur.left_offset != -1)
							pre.right_offset = cur.left_offset;
						else if (cur.right_offset != -1)
							pre.right_offset = cur.right_offset;
						else if (cur.right_offset == -1 && cur.left_offset == -1)
							pre.right_offset = -1;
					}
					fseek(fp, prevos, SEEK_SET);
					fwrite(&pre, sizeof(node_t), 1, fp);
				}
				else
				{
					if (cur.left_offset != -1)
					{
						tree.root = cur.left_offset;
					}
					else if (cur.right_offset != -1)
					{
						tree.root = cur.right_offset;
					}
					else if (cur.left_offset == -1 && cur.right_offset == -1)
					{
						tree.root = -1;
					}
					fseek(fp, 0, SEEK_SET);
					fwrite(&tree, sizeof(tree_t), 1, fp);
				}
				cur.key = -1;
				cur.left_offset = -1;
				cur.right_offset = -1;
				fseek(fp, curos, SEEK_SET);
				fwrite(&cur, sizeof(node_t), 1, fp);
				add_freehead(fp, curos);
			}
			break;

		case 2:
			{
				int ppos, isos;
				node_t pp, is;
				int flag = 0;
				fseek(fp, cur.left_offset, SEEK_SET);
				isos = ftell(fp);
				fread(&is, sizeof(node_t), 1, fp);
				if (is.right_offset == -1)
				{
					flag = 1;
				}

				while (is.right_offset != -1)
				{
					ppos = isos;
					pp = is;
					fseek(fp, is.right_offset, SEEK_SET);
					isos = ftell(fp);
					fread(&is, sizeof(node_t), 1, fp);
				}
				if (!flag)
				{
					pp.right_offset = is.left_offset;
				}
				else
				{
					cur.left_offset = is.left_offset;
				}
				cur.key = is.key;
				is.key = -1;
				is.left_offset = -1;
				fseek(fp, curos, SEEK_SET);
				fwrite(&cur, sizeof(node_t), 1, fp);
				if (!flag)
				{
					fseek(fp, ppos, SEEK_SET);
					fwrite(&pp, sizeof(node_t), 1, fp);
				}
				fseek(fp, isos, SEEK_SET);
				fwrite(&is, sizeof(node_t), 1, fp);
				add_freehead(fp, isos);
			}
			break;
	}
}

void add_freehead(FILE *fp, int curos)     // links all the free nodes in the file
{
	tree_t tree;
	fseek(fp, 0, SEEK_SET);
	fread(&tree, sizeof(tree_t), 1, fp);
	if (tree.free_head == -1)
	{
		tree.free_head = curos;
	}
	else
	{
		int tos;
		node_t tra;
		fseek(fp, tree.free_head, SEEK_SET);
		tos = ftell(fp);
		fread(&tra, sizeof(node_t), 1, fp);
		while (tra.left_offset != -1)
		{
			fseek(fp, tra.left_offset, SEEK_SET);
			fread(&tra, sizeof(node_t), 1, fp);
		}
		tra.left_offset = curos;
		fseek(fp, tos, SEEK_SET);
		fwrite(&tra, sizeof(node_t), 1, fp);
	}
	fseek(fp, 0, SEEK_SET);
	fwrite(&tree, sizeof(tree_t), 1, fp);
}
