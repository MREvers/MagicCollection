using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CollectorsFrontEnd
{
    public delegate void MenuAction();

    public interface IMenuBarComponent: IComponent
    {

        List<Tuple<string, MenuAction>> GetMenuActions();

    }
}
