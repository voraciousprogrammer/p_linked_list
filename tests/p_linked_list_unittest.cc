#include "gtest/gtest.h"

extern "C"
{
#include "p_linked_list.h"
}

int number_of_deallocations = 0;

void deallocator(const void *data)
{
        (void)data;
        ++number_of_deallocations;
}

// Create a list and destroy it after.
TEST(BasicLinkedListUnitTests, CreateAndDestroyList)
{
        p_linked_list_t *list;

        // Create a new list with a deallocator and comparefn set to NULL.
        list = p_linked_list_create(NULL, NULL);
        EXPECT_TRUE(list == NULL);

        // Create a new list with the comparefn set to NULL.
        list = p_linked_list_create(deallocator, NULL);
        EXPECT_TRUE(list != NULL);

        // Deallocat a NULL list.
        p_linked_list_destroy(NULL);

        // Deallocate the list.
        p_linked_list_destroy(list);
}

// Verify that the p_linked_list_is_empty function can operate on NULL.
TEST(BasicLinkedListUnitTests, IsEmptyOnNull)
{
        EXPECT_EQ(1, p_linked_list_is_empty(NULL));
}

// Verify that the deallocator is run once for each element in the list.
TEST(BasicLinkedListUnitTests, VerifyDeallocator)
{
        p_linked_list_t *list;
        int e1 = 10;
        int e2 = 20;
        int e3 = 30;

        // Create a new list with the comparefn set to NULL.
        list = p_linked_list_create(deallocator, NULL);
        EXPECT_TRUE(list != NULL);

        EXPECT_EQ(1, p_linked_list_add_element(list, &e1, OR_TAIL));
        EXPECT_EQ(1, p_linked_list_add_element(list, &e2, OR_TAIL));
        EXPECT_EQ(1, p_linked_list_add_element(list, &e3, OR_TAIL));

        // Deallocate the list.
        number_of_deallocations = 0;
        p_linked_list_destroy(list);
        EXPECT_EQ(3, number_of_deallocations);
}

class BasicAddAndRemoveElementsFromList : public testing::Test {

        protected:
                virtual void SetUp()
                {
                        list = p_linked_list_create(deallocator, NULL);
                        EXPECT_TRUE(list != NULL);
                }

                virtual void TearDown()
                {
                        p_linked_list_destroy(list);
                }

                p_linked_list *list;
};

// Add and remove a single element to a NULL list.
TEST_F(BasicAddAndRemoveElementsFromList, AddAndRemoveElementToNULLLIST)
{
        int e = 10;

        EXPECT_EQ(0, p_linked_list_add_element(NULL, &e, OR_HEAD));

        EXPECT_EQ(NULL, (int *)p_linked_list_remove_element(NULL, OR_HEAD));
}

// Add and remove a single element to/from the front of the list.
TEST_F(BasicAddAndRemoveElementsFromList, AddAndRemoveElement)
{
        int e = 10;

        EXPECT_EQ(1, p_linked_list_add_element(list, &e, OR_HEAD));

        EXPECT_EQ(10, *(int *)p_linked_list_remove_element(list, OR_HEAD));
}

// Add and remove 2 elements to/from the front of the list.
TEST_F(BasicAddAndRemoveElementsFromList, AddAndRemoveElementsHead)
{
        int e1 = 10;
        int e2 = 100;

        EXPECT_EQ(1, p_linked_list_add_element(list, &e1, OR_HEAD));
        EXPECT_EQ(1, p_linked_list_add_element(list, &e2, OR_HEAD));

        EXPECT_EQ(100, *(int *)p_linked_list_remove_element(list, OR_HEAD));
        EXPECT_EQ(10, *(int *)p_linked_list_remove_element(list, OR_HEAD));
}

// Add an element and remove it to/from the head of the list
// and then verify that the list is empty.
TEST_F(BasicAddAndRemoveElementsFromList, AddAndRemoveElementTestEmpty)
{
        int e = 10;

        EXPECT_EQ(1, p_linked_list_add_element(list, &e, OR_HEAD));

        EXPECT_EQ(10, *(int *)p_linked_list_remove_element(list, OR_HEAD));

        EXPECT_EQ(1, p_linked_list_is_empty(list));
}

TEST_F(BasicAddAndRemoveElementsFromList, RemoveFromEmptyList)
{
        EXPECT_EQ(NULL, (int *)p_linked_list_remove_element(list, OR_HEAD));
        EXPECT_EQ(NULL, (int *)p_linked_list_remove_element(list, OR_TAIL));
}

// Add and remove 2 elements to/from the back of the list.
TEST_F(BasicAddAndRemoveElementsFromList, AddAndRemoveElementsTail)
{
        int e1 = 10;
        int e2 = 100;

        EXPECT_EQ(1, p_linked_list_add_element(list, &e1, OR_TAIL));
        EXPECT_EQ(1, p_linked_list_add_element(list, &e2, OR_TAIL));

        EXPECT_EQ(100, *(int *)p_linked_list_remove_element(list, OR_TAIL));
        EXPECT_EQ(10, *(int *)p_linked_list_remove_element(list, OR_TAIL));
}

// Add 2 elements to head and remove 2 elements rom the back of the list.
TEST_F(BasicAddAndRemoveElementsFromList, AddHeadAndRemoveTail)
{
        int e1 = 10;
        int e2 = 100;

        EXPECT_EQ(1, p_linked_list_add_element(list, &e1, OR_HEAD));
        EXPECT_EQ(1, p_linked_list_add_element(list, &e2, OR_HEAD));

        EXPECT_EQ(10, *(int *)p_linked_list_remove_element(list, OR_TAIL));
        EXPECT_EQ(100, *(int *)p_linked_list_remove_element(list, OR_TAIL));
}

// Add 2 elements to tail and remove 2 elements from the head of the list.
TEST_F(BasicAddAndRemoveElementsFromList, AddTailAndRemoveHead)
{
        int e1 = 10;
        int e2 = 100;

        EXPECT_EQ(1, p_linked_list_add_element(list, &e1, OR_TAIL));
        EXPECT_EQ(1, p_linked_list_add_element(list, &e2, OR_TAIL));

        EXPECT_EQ(10, *(int *)p_linked_list_remove_element(list, OR_HEAD));
        EXPECT_EQ(100, *(int *)p_linked_list_remove_element(list, OR_HEAD));
}

// Add one element to the head and one element to the tail and
// remove both from the tail.
TEST_F(BasicAddAndRemoveElementsFromList, AddHeadTailAndRemoveTail)
{
        int e1 = 10;
        int e2 = 100;

        EXPECT_EQ(1, p_linked_list_add_element(list, &e1, OR_HEAD));
        EXPECT_EQ(1, p_linked_list_add_element(list, &e2, OR_TAIL));

        EXPECT_EQ(100, *(int *)p_linked_list_remove_element(list, OR_TAIL));
        EXPECT_EQ(10, *(int *)p_linked_list_remove_element(list, OR_TAIL));
}
