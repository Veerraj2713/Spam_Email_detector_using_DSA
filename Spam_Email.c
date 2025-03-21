#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define ALPHABET_SIZE 26
#define THRESHOLD 1
#define MAX_LEVEL 20
#define SIZE 50

struct stack
{
    int data[SIZE];
    int top;
};

typedef struct SkipNode
{
    char email[256];
    struct SkipNode* forward[MAX_LEVEL];
} SkipNode;

typedef struct SkipList
{
    SkipNode* header;
    int level;
} SkipList;

void computeLPSArray(char* pat, int M, int* lps);

struct dt
{
    int d,m,y;
};


struct email
{
    char reciever[1000];
    char sender[1000];
    struct dt date;
    char text[10000];

};

struct TrieNode
{
    struct TrieNode* children[ALPHABET_SIZE];
    int isLeaf;
};

void push(struct stack* s, int ele);
int isfull(struct stack* s);
int pop(struct stack* s);
int isempty(struct stack* s);
void display(struct stack s);
int stacktop(struct stack* s);
void stackinitialize(struct stack* s);

struct stack s;
struct stack spam;
struct stack ham;
int c=0;
int sw=0;
int h=0;

void reademail(struct email* e)
{
    scanf("%*c");
    printf("Enter text");
    gets(e->text);
    printf("Enter sender");
    gets(e->sender);
    printf("Enter reciver");
    gets(e->reciever);
    printf("Enter date");
    scanf("%d%*c%d%*c%d",&e->date.d,&e->date.m,&e->date.y);
    c++;
    push(&s,c);
}


int hash1(char * str,int n)
{
    char c;
    int hash=5381;
    while(*str)
    {
        c=*str;
        hash=hash * 33 + c;
        str=str+1;
    }
    return abs(hash%n);
}

int hash2(char * str,int n)
{

    char c;
    int hash=0;
    while(*str)
    {
        c=*str;
        hash = c + (hash << 6) + (hash << 16) - hash;
        str=str+1;
    }
    return abs(hash%n);
}


void setbloom(int * arr)
{
    char str[1000];
    int m;
    int n;

    char * spamWords[]= {"free","lottery","billion","guaranteed",
                         "giveaway","miracle","risk-free",
                         "affordable","instant","urgent",
                         "confidentiality","passwords","urgent","congratulations",
                         "bonus","discount","prize","luxury",
                         "opportunity","hurray"
                        };


    int size=sizeof(spamWords)/sizeof(spamWords[0]);
    printf("size=%d\n",size);
    for(int i=0; i<size; i++)
    {
        strcpy(str,spamWords[i]);
        m=hash1(str,100);
        n=hash2(str,100);
        if(arr[m]==0)
            arr[m]=1;
        if(arr[n]==0)
            arr[n]=1;


    }
}
int hasChild(struct TrieNode*cur)
{
    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        if (cur->children[i])
        {
            return 1;
        }
    }
    return 0;
}
struct TrieNode*getNode(void)
{
    struct TrieNode*p =(struct TrieNode*)malloc(sizeof(struct TrieNode));
    if (p)
    {
        p->isLeaf = 0;
        for (int i=0; i<ALPHABET_SIZE; i++)
        {
            p->children[i]=NULL;
        }
    }
    return p;
}

void insert(struct TrieNode** root,  char* key)
{
    struct TrieNode*cur=*root;
    int index;
    for (int i=0; key[i]!='\0'; i++)
    {
        index=key[i]-'a';
        if (!cur->children[index])
        {
            cur->children[index]=getNode();
        }
        cur=cur->children[index];
    }
    cur->isLeaf = 1;
}

int search(struct TrieNode** root, char* key)
{
    struct TrieNode*cur=*root;

    for (int i=0; key[i]; i++)
    {
        if (cur->children[key[i]-'a']!= NULL)
        {
            cur=cur->children[key[i]-'a'];
        }
        else
        {
            return 0;
        }
    }
    if(cur->isLeaf==1)
        return 1;
    else
        return 0;
}

void deleteKey(struct TrieNode**root,char*key)
{
    struct TrieNode*cur=*root;
    int i = 0;
    while (key[i])
    {
        int index=key[i]-'a';
        if (cur->children[index]==NULL)
        {
            return;
        }
        cur=cur->children[index];
        i++;
    }
    if (cur->isLeaf)
    {
        cur->isLeaf = 0;
    }
    else
    {
        return;
    }

    for (i=strlen(key)-1; i>= 0; i--)
    {
        cur = *root;
        for (int j = 0; key[j] &&j< i; j++)
        {
            int index=key[j]-'a';
            cur=cur->children[index];
        }
        int index = key[i]-'a';
        if (cur->children[index]&&!cur->children[index]->isLeaf&&!hasChild(cur->children[index]))
        {
            free(cur->children[index]);
            cur->children[index]=NULL;
        }
        else
        {
            break;
        }
    }
}


void displayroot(struct TrieNode*root,char *str,int index)
{

    if(root->isLeaf==1)
    {


        str[index]='\0';

        printf("%s\n",str);

    }




    for(int i=0; i<ALPHABET_SIZE; i++)
    {

        if(root->children[i]!=NULL)
        {

            str[index]=i+'a';

            displayroot(root->children[i],str,index+1);


        }



    }


}

void detectspamword(struct TrieNode**root,char*str,int index,int* hash,int *c);

int preprocess(struct email *e,int *hash)
{
    struct TrieNode *root=getNode();
    int i,j=0,p;
    char test[1000];
    char str[1000];
    char stopwords[][1000]=
    {
        "i", "me", "my", "myself", "we", "our", "ours", "ourselves", "you", "your", "yours", "yourself", "yourselves",
        "he", "him", "his", "himself", "she", "her", "hers", "herself", "it", "its", "itself", "they", "them", "their",
        "theirs", "themselves", "what", "which", "who", "whom", "this", "that", "these", "those", "am", "is", "are", "was",
        "were", "be", "been", "being", "have", "has", "had", "having", "do", "does", "did", "doing", "a", "an", "the",
        "and", "but", "if", "or", "because", "as", "until", "while", "of", "at", "by", "for", "with", "about", "against",
        "between", "into", "through", "during", "before", "after", "above", "below", "to", "from", "up", "down", "in",
        "out", "on", "off", "over", "under", "again", "further", "then", "once", "here", "there", "when", "where", "why",
        "how", "all", "any", "both", "each", "few", "more", "most", "other", "some", "such", "no", "nor", "not", "only",
        "own", "same", "so", "than", "too", "very", "s", "t", "can", "will", "just", "don", "should", "now"
    };
    int n=sizeof(stopwords)/sizeof(stopwords[0]);
    printf("in preprocess array\n");
    puts(e->text);
    strcpy(str,e->text);
    printf("in preprocess\n");
    puts(str);

    p=0;
    while (str[p])
    {
        if (str[p]==' ')
        {
            p++;
        }
        j = 0;
        while ((str[p]!=' ') && (str[p]!='\0'))
        {

            test[j++] = tolower(str[p++]);
        }
        if (j)
        {

            test[j] = '\0';
            insert(&root, test);

        }

    }




    for (int m=0; m<n; m++)
    {
        if(search(&root,stopwords[m]))
        {

            deleteKey(&root,stopwords[m]);
        }
    }

    int c=0;

    detectspamword(&root,str,0,hash,&c);
    free(root);
    root=NULL;
    return c;


}

void displayMail(struct email * e)
{
    puts(e->sender);
    printf("\n");
    puts(e->reciever);
    printf("\n");
    printf("%d/%d/%d\n",e->date.d,e->date.m,e->date.y);
    puts(e->text);
    printf("\n");
}

void detectspamword(struct TrieNode**root,char*str,int index,int* hash,int *c)
{
    if((*root)->isLeaf==1)
    {

        int m,n;
        str[index]='\0';
        puts(str);
        printf("\n");
        m=hash1(str,100);
        n=hash2(str,100);

        if(hash[m]==1 && hash[n]==1)
        {
            (*c)++;
        }

    }




    for(int i=0; i<ALPHABET_SIZE; i++)
    {

        if((*root)->children[i]!=NULL)
        {

            str[index]=i+'a';

            detectspamword(&((*root)->children[i]),str,index+1,hash,c);


        }



    }


}
void checkSpam(struct email * arr,int k,int * hash)
{
    char str[1000];
    int c;
    FILE * fp_spam;
    FILE * fp_ham;
    fp_spam=fopen("spam.txt","wb");
    fp_ham= fopen("ham.txt","wb");



    for(int i=0; i<k; i++)
    {

        c=preprocess(&arr[i],hash);

        printf("count=%d\n",c);
        if((c)>=THRESHOLD)
        {
            push(&spam, i+1);
            fwrite(&arr[i],sizeof(arr[i]),1,fp_spam);
        }
        else
        {
            printf("write ham\n");
            push(&ham, i+1);
            fwrite(&arr[i],sizeof(arr[i]),1,fp_ham);
        }
    }

    fclose(fp_spam);
    fclose(fp_ham);
}

void displayspam(void)
{
    printf("Spam emails are\n");
    FILE * fp=fopen("spam.txt","rb");
    struct email e;
    while(fread(&e,sizeof(e),1,fp)==1)
    {
        {
            displayMail(&e);
        }
    }
    fclose(fp);
}

void displayham(void)
{
    printf("Ham emails are\n");
    FILE * fp=fopen("ham.txt","rb");
    struct email e;
    while(fread(&e,sizeof(e),1,fp)==1)
    {
        {
            displayMail(&e);
        }
    }
    fclose(fp);
}


int cmp(struct dt min,struct dt i)
{
    if(i.y>min.y)
        return 0;
    else if(i.m>min.m)
        return 0;
    else if(i.d>min.d)
        return 0;
    else
        return 1;
}


void heapify(struct email * arr,int n,int i)
{
    int min=i;
    struct email temp;

    int left=2*i+1;
    int right=2*i+2;

    if(left<n && cmp(arr[min].date,arr[left].date))
        min=left;

    if(right<n && cmp(arr[min].date,arr[right].date))
        min=right;

    if(min!=i)
    {
        temp=arr[i];
        arr[i]=arr[min];
        arr[min]=temp;
        heapify(arr,n,min);
    }
}


void heapsort(struct email * arr,int n)
{
    struct email temp;
    for(int i=(n/2)-1; i>=0; i--)
        heapify(arr,n,i);


    for (int i = n - 1; i > 0; i--)
    {
        temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;
        heapify(arr, i, 0);
    }
}

int KMPSearch(char* pat, char* txt)
{
    int M = strlen(pat);
    int N = strlen(txt);

    int lps[M];

    computeLPSArray(pat, M, lps);

    int i = 0;
    int j = 0;
    while ((N - i) >= (M - j))
    {
        if (pat[j] == txt[i])
        {
            j++;
            i++;
        }
        if (j == M)
        {
            printf("phishing email\n");
            j = lps[j - 1];
            return 1;
        }

        else if (i < N && pat[j] != txt[i])
        {
            if (j != 0)
                j = lps[j - 1];
            else
                i = i + 1;
        }
    }
    return 0;
}

void computeLPSArray(char* pat, int M, int* lps)
{
    int len = 0;

    lps[0] = 0;

    int i = 1;
    while (i < M)
    {
        if (pat[i] == pat[len])
        {
            len++;
            lps[i] = len;
            i++;
        }
        else
        {
            if (len != 0)
            {
                len = lps[len - 1];
            }
            else
            {
                lps[i] = 0;
                i++;
            }
        }
    }
}

SkipNode* createSkipNode(const char* email, int level)
{
    SkipNode* newNode = (SkipNode*)malloc(sizeof(SkipNode));
    strncpy(newNode->email, email, sizeof(newNode->email) - 1);
    for (int i = 0; i < level; i++)
    {
        newNode->forward[i] = NULL;
    }
    return newNode;
}

SkipList* createSkipList()
{
    SkipList* list = (SkipList*)malloc(sizeof(SkipList));
    list->header = createSkipNode("", MAX_LEVEL);
    list->level = 1;
    return list;
}

int randomLevel()
{
    int level = 1;
    while (rand() % 2 == 0 && level < MAX_LEVEL)
    {
        level++;
    }
    return level;
}

void insertEmail(SkipList* list, const char* email)
{
    SkipNode* update[MAX_LEVEL];
    SkipNode* current = list->header;

    for (int i = list->level - 1; i >= 0; i--)
    {
        while (current->forward[i] != NULL && strcmp(current->forward[i]->email, email) < 0)
        {
            current = current->forward[i];
        }
        update[i] = current;
    }

    int newLevel = randomLevel();

    if (newLevel > list->level)
    {
        for (int i = list->level; i < newLevel; i++)
        {
            update[i] = list->header;
        }
        list->level = newLevel;
    }

    SkipNode* newNode = createSkipNode(email, newLevel);

    for (int i = 0; i < newLevel; i++)
    {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }
}

int searchEmail(SkipList* list, char* email)
{
    SkipNode* current = list->header;

    for (int i = list->level - 1; i >= 0; i--)
    {
        while (current->forward[i] != NULL && strcmp(current->forward[i]->email, email) < 0)
        {
            current = current->forward[i];
        }
    }

    if (current->forward[0] != NULL && strcmp(current->forward[0]->email, email) == 0)
    {
        return 1;
    }

    return 0;
}

int isfull(struct stack* s)
{
    if (s->top == SIZE - 1)
        return 1;
    return 0;
}

void push(struct stack* s, int ele)
{
    if (isfull(s))
    {
        printf("Stack Overflow\n");
        return;
    }

    s->top++;
    s->data[s->top] = ele;
}

int pop(struct stack* s)
{
    int ele;
    if (isempty(s))
    {
        printf("Stack Underflow\n");
        return -1;
    }
    ele = s->data[s->top];
    s->top--;
    return ele;
}

int isempty(struct stack* s)
{
    if (s->top == -1)
        return 1;
    return 0;
}

int stacktop(struct stack* s)
{
    return (s->data[s->top]);
}

void display(struct stack s)
{
    int i;
    if (isempty(&s))
    {
        printf("Stack underflow\n");
        return;
    }
    printf("Stack elements: ");
    for (i = s.top; i >= 0; i--)
        printf("%d ", s.data[i]);
    printf("\n");
}

void stackinitialize(struct stack* s)
{
    s->top = -1;
}


int main()
{
    int n;
    int ch;
    int c;
    int hash[100]= {0};
    stackinitialize(&s);
    stackinitialize(&spam);
    stackinitialize(&ham);
    SkipList* emailSkipList = createSkipList();

    char *sendersid[] =
    {
        "yourmail@gmail.com",
        "xrumer888@outlook.com",
        "ericjonesmyemail@gmail.com",
        "callvisvetlana@list.ru",
        "georginahaynes620@gmail.com",
        "hifromwilbur@gmail.com",
        "meganatkinson149@gmail.com",
        "lukinovich20221355@gmail.com",
        "mohamadabdulraman447@gmail.com",
        "help@gwmetabitt.com",
        "lionel@lioneldarnell.net",
        "libbyevans461@gmail.com",
        "alexpopov716253@gmail.com",
        "inet4747@outlook.com",
        "froeach12@rezka.wtf",
        "growthpartners13@gmail.com",
        "seosubmitter@mail.com",
        "jerrypark002@outlook.com"
    };

    printf("Enter number of emails to handle with");
    scanf("%d",&n);
    struct email e;
    FILE * fp;
    fp=fopen("SpamEmail.txt","wb");
    if(fp==NULL)
    {
        printf("file couldn't open\n");
        return 0;
    }
    for(int i=0; i<n; i++)
    {
        reademail(&e);
        fwrite(&e,sizeof(e),1,fp);
    }
    printf("the number of emails are: %d\n", stacktop(&s));
    display(s);
    fclose (fp);

    struct email emailarr[100];
    int k=0;
    fp=fopen("SpamEmail.txt","rb");
    printf("including in array\n");

    while(fread(&e,sizeof(e),1,fp)==1)
    {
        emailarr[k++]=e;
    }
    fclose(fp);
    printf("array elements\n");
    printf("k=%d\n",k);

    struct TrieNode*root=NULL;
    while(1)
    {
        printf("Displaying choices\n");
        printf("0.Exit\n");
        printf("1.Setting up hash function\n");
        printf("2.Identify spam emails and store it in another file\n");
        printf("3.To check the spam email using senders address\n");
        printf("4.Checking whether an email contains phishing URLs\n");
        printf("5.Display spam emails\n");
        printf("6.Display ham emails\n");
        printf("7.Display emails in the inbox\n");

        printf("Enter choice ");
        scanf("%d",&ch);

        switch(ch)
        {
        case 0:
            return 0;

        case 1:
            setbloom(hash);
            break;

        case 2:
            checkSpam(emailarr,k,hash);
            printf("number of spam emails are: %d\n",stacktop(&spam));
            printf("number of ham emails are: %d\n",stacktop(&ham));
            break;

        case 3:
            for(int i=0; i<sizeof(sendersid)/sizeof(sendersid[0]); i++) {
                insertEmail(emailSkipList, sendersid[i]);
            }

            for(int i=0; i<k; i++)
            {
                if (searchEmail(emailSkipList,emailarr[i].sender))
                {
                    puts(emailarr[i].sender);
                    printf("This email is in the blacklist!\n");
                }
                else
                {
                    printf("This email is not in the blacklist.\n");
                }
            }
            break;

        case 4:
            for(int i=0; i<k; i++)
            {
                if(KMPSearch("http://federmacedoadv.com.br/3f/aze/ab51e2e319e51502f416dbe46b773a5e/?cmd=_home&amp;dispatch=11004d58f5b74f8dc1e7c2e8dd4105e811004d58f5b74f8dc1e7c2e8dd4105e8@phishing.website.html",emailarr[i].text) ||
                        KMPSearch("http://125.98.3.123/fake.html%E2%80%9D",emailarr[i].text) ||
                        KMPSearch("http://88.204.202.98/2/paypal.ca/index.html%E2%80%9D",emailarr[i].text) ||
                        KMPSearch("http://portal.hud.ac.uk/%E2%80%9D",emailarr[i].text) ||
                        KMPSearch("http://www.legitimate.com//http://www.phishing.com%E2%80%9D",emailarr[i].text) ||
                        KMPSearch("http://www.confirme-paypal.com/",emailarr[i].text) ||
                        KMPSearch("http://www.hud.ac.uk/students/",emailarr[i].text) ||
                        KMPSearch("http://https-www-paypal-it-webapps-mpp-home.soft-hair.com/",emailarr[i].text) ||
                        KMPSearch("www.dbuk.net",emailarr[i].text) ||
                        KMPSearch("http://wap.xxxmobilemovieclub.com?n=QJKGIGHJJGCBL",emailarr[i].text))
                {
                    printf("Email %d contains phishing URL\n", i+1);
                }
            }
            break;

        case 5:
            displayspam();
            break;

        case 6:
            displayham();
            break;

        case 7:
            heapsort(emailarr,k);
            for(int i=0; i<k; i++)
            {
                displayMail(&emailarr[i]);
            }
            break;

        default:
            printf("Invalid choice\n");
        }
    }

    return 0;
}
