#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) test_malloc(sizeof(struct list_head));
    if (head) {
        INIT_LIST_HEAD(head);
    }
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        list_del(&(entry->list));
        test_free(entry->value);
        test_free(entry);
    }
    test_free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *ele = (element_t *) test_malloc(sizeof(element_t));
    if (!ele)
        return false;

    ele->value = (char *) test_malloc(strlen(s) + 1);
    if (!ele->value) {
        test_free(ele);
        return false;
    }
    strncpy(ele->value, s, strlen(s) + 1);
    list_add(&ele->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *ele = (element_t *) test_malloc(sizeof(element_t));
    if (!ele)
        return false;

    ele->value = (char *) test_malloc(strlen(s) + 1);
    if (!ele->value) {
        test_free(ele);
        return false;
    }
    strncpy(ele->value, s, strlen(s) + 1);
    list_add_tail(&ele->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node_tmp = head->next;
    element_t *ele_tmp = container_of(node_tmp, element_t, list);
    if (sp) {
        size_t size = strlen(ele_tmp->value) + 1;
        size = bufsize < size ? bufsize : size;
        strncpy(sp, ele_tmp->value, size);
        sp[size - 1] = '\0';
    }

    list_del(node_tmp);
    return ele_tmp;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node_tmp = head->prev;
    element_t *ele_tmp = container_of(node_tmp, element_t, list);
    if (sp) {
        size_t size = strlen(ele_tmp->value) + 1;
        size = bufsize < size ? bufsize : size;
        strncpy(sp, ele_tmp->value, size);
        sp[size - 1] = '\0';
    }

    list_del(node_tmp);
    return ele_tmp;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || !head->next)
        return false;
    struct list_head *forward = head->next, *backward = head->prev;
    while (forward != backward && forward->next != backward) {
        forward = forward->next;
        backward = backward->prev;
    }
    element_t *tmp = container_of(forward, element_t, list);
    list_del(forward);
    q_release_element(tmp);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    element_t *iter, *safe;
    bool dup = false;
    list_for_each_entry_safe (iter, safe, head, list) {
        if (&safe->list != head && strcmp(iter->value, safe->value) == 0) {
            dup = true;
            list_del(&iter->list);
            q_release_element(iter);
        } else if (dup) {
            list_del(&iter->list);
            q_release_element(iter);
            dup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || q_size(head) <= 1)
        return;
    struct list_head *curr, *prev;
    prev = head->next;
    curr = prev->next;
    while (prev != head && curr != head) {
        prev->prev->next = curr;
        curr->next->prev = prev;

        curr->prev = prev->prev;
        prev->prev = curr;

        prev->next = curr->next;
        curr->next = prev;

        prev = prev->next;
        curr = prev->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *tmp = NULL, *node = NULL;
    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
    list_for_each (node, head) {
        tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    return;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head)
        return 0;
    if (list_empty(head))
        return 1;

    int count = 1;
    struct list_head *node = head->prev;
    element_t *curr = container_of(node, element_t, list), *prev;
    char *min = curr->value;
    while (node->prev != head) {
        prev = container_of(node->prev, element_t, list);
        if (strcmp(prev->value, min) > 0) {
            list_del(node->prev);
            q_release_element(prev);
        } else {
            min = prev->value;
            count++;
            node = node->prev;
        }
    }
    return count;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head)
        return 0;
    if (list_empty(head))
        return 1;

    int count = 1;
    struct list_head *node = head->prev;
    element_t *curr = container_of(node, element_t, list), *prev;
    char *max = curr->value;
    while (node->prev != head) {
        prev = container_of(node->prev, element_t, list);
        if (strcmp(prev->value, max) < 0) {
            list_del(node->prev);
            q_release_element(prev);
        } else {
            max = prev->value;
            count++;
            node = node->prev;
        }
    }
    return count;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/

    return 0;
}
