#include <Terminate.h>

class foo
{
  public:
    foo() { foo1(); }

  private:
    void foo1() { foo2(); }
    void foo2() { foo3(); }
    void foo3() { foo4(); }
    void foo4() { crash(); }
    void crash() { char * p = NULL; *p = 0; }
};

int main(int argc, char ** argv)
{
  Error::TerminalCatcher::init();


  foo * f = new foo();
  return 0;
}
