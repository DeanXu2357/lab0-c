#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *li, *tmp;
    list_for_each_safe (li, tmp, head) {
        element_t *el = list_entry(li, element_t, list);
        list_del_init(&el->list);
        free(el->value);
        free(el);
    }

    free(head);
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *n = malloc(sizeof(element_t));
    if (!n)
        return false;

    n->value = strdup(s);
    if (!n->value) {
        free(n);
        return false;
    }

    INIT_LIST_HEAD(&n->list);

    list_add(&n->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *n = malloc(sizeof(element_t));
    if (!n)
        return false;

    n->value = strdup(s);
    if (!n->value) {
        free(n);
        return false;
    }

    INIT_LIST_HEAD(&n->list);

    list_add_tail(&n->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || !sp || bufsize == 0)
        return NULL;

    if (list_empty(head))
        return NULL;

    element_t *h = list_first_entry(head, element_t, list);
    list_del(&h->list);

    strncpy(sp, h->value, bufsize);
    return h;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || !sp || bufsize == 0)
        return NULL;

    if (list_empty(head))
        return NULL;

    element_t *t = list_last_entry(head, element_t, list);
    list_del(&t->list);

    strncpy(sp, t->value, bufsize);
    return t;
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
    if (!head || list_empty(head))
        return false;

    struct list_head *mid;
    if (list_is_singular(head)) {
        mid = head->next;
    } else {
        struct list_head *slow = head->next;
        struct list_head *fast = head->next;

        while (fast != head && fast->next != head) {
            fast = fast->next->next;
            slow = slow->next;
        }

        mid = slow;
    }

    list_del_init(mid);
    element_t *e = list_entry(mid, element_t, list);
    free(e->value);
    free(e);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head) || list_is_singular(head))
        return false;

    struct list_head tmp;
    INIT_LIST_HEAD(&tmp);

    list_splice_init(head, &tmp);

    element_t *entry, *safe;
    safe = NULL;
    list_for_each_entry_safe (entry, safe, &tmp, list) {
        element_t *j, *s;
        s = NULL;
        bool is_dup = false;
        list_for_each_entry_safe (j, s, &tmp, list)
            if (j != entry && strcmp(j->value, entry->value) == 0)
                is_dup = true;

        if (!is_dup) {
            list_del(&entry->list);
            list_add_tail(&entry->list, head);
        }
    }

    struct list_head *n, *sn;
    list_for_each_safe (n, sn, &tmp) {
        list_del_init(n);
        element_t *d = list_entry(n, element_t, list);
        free(d->value);
        free(d);
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head tmp;
    INIT_LIST_HEAD(&tmp);

    list_splice_init(head, &tmp);

    int count = 0;
    while (!list_empty(&tmp)) {
        struct list_head *last = head->prev;

        struct list_head *tail = tmp.next;
        list_del(tail);
        list_add_tail(tail, head);

        if (count % 2 != 0) {
            list_del(last);
            list_add_tail(last, head);
        }

        count++;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head tmp;
    INIT_LIST_HEAD(&tmp);

    list_splice_init(head, &tmp);

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, &tmp) {
        list_del(node);
        list_add(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head copy;
    INIT_LIST_HEAD(&copy);

    list_splice_init(head, &copy);

    struct list_head tmp;
    INIT_LIST_HEAD(&tmp);

    int count = 1;
    struct list_head *n, *s;
    s = NULL;
    list_for_each_safe (n, s, &copy) {
        list_del(n);
        list_add(n, &tmp);

        if (count % k == 0)
            list_splice_tail_init(&tmp, head);

        count++;
    }

    if (!list_empty(&tmp)) {
        n = NULL;
        s = NULL;
        q_reverse(&tmp);
        list_for_each_safe (n, s, &tmp) {
            list_del(n);
            list_add_tail(n, head);
        }
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head tmp;
    INIT_LIST_HEAD(&tmp);

    list_splice_init(head, &tmp);

    while (!list_empty(&tmp)) {
        element_t *greatest = list_entry(tmp.next, element_t, list);

        element_t *entry, *safe;
        safe = NULL;
        list_for_each_entry_safe (entry, safe, &tmp, list) {
            if (strcmp(entry->value, greatest->value) >= 0) {
                greatest = entry;
            }
        }

        list_del(&greatest->list);
        list_add(&greatest->list, head);
    }

    if (descend) {
        q_reverse(head);
    }

    return;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;

    struct list_head copy;
    INIT_LIST_HEAD(&copy);

    list_splice_init(head, &copy);

    element_t *entry, *safe;
    safe = NULL;
    list_for_each_entry_safe (entry, safe, &copy, list) {
        bool should_keep = true;

        for (struct list_head *j = entry->list.next; j != &copy; j = j->next) {
            if (strcmp(list_entry(j, element_t, list)->value, entry->value) <
                0) {
                should_keep = false;
                break;
            }
        }

        if (should_keep) {
            list_del(&entry->list);
            list_add_tail(&entry->list, head);
        }
    }

    q_free(&copy);

    int len = 0;
    struct list_head *n;
    list_for_each (n, head)
        len++;

    return len;
}

/* Remove every node which has a node with a strictly greater value anywhere
 * to the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;

    struct list_head copy;
    INIT_LIST_HEAD(&copy);

    list_splice_init(head, &copy);

    element_t *entry, *safe;
    safe = NULL;
    list_for_each_entry_safe (entry, safe, &copy, list) {
        bool should_keep = true;

        for (struct list_head *j = entry->list.next; j != &copy; j = j->next) {
            if (strcmp(list_entry(j, element_t, list)->value, entry->value) >
                0) {
                should_keep = false;
                break;
            }
        }

        if (should_keep) {
            list_del(&entry->list);
            list_add_tail(&entry->list, head);
        }
    }

    q_free(&copy);

    int len = 0;
    struct list_head *n;
    list_for_each (n, head)
        len++;

    return len;
}

/* Merge all the queues into one sorted queue, which is in
 * ascending/descending order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
