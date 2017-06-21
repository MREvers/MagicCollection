using CollectorsFrontEnd.InterfaceModels;
using CollectorsFrontEnd.StoreFrontSupport;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace CollectorsFrontEnd.Views.ViewComponents
{
    /// <summary>
    /// Interaction logic for VCIGroupList.xaml
    /// Naming Clarification
    /// VC => View Component
    /// I => Independent; meaning it has its own data model/view model.
    /// </summary>
    public partial class VCIGroupList : UserControl
    {

        #region Properties

        public VMVCIGroupList VMVCIGroupList { get; set; }

        #endregion
        

        public VCIGroupList(string GroupName, List<CardModel> GroupList)
        {
            InitializeComponent();
            this.VMVCIGroupList = new VMVCIGroupList(Model: this);
            DataContext = this.VMVCIGroupList;

            refreshViewModel(GroupName, GroupList);
        }

        private void refreshViewModel(string GroupName, List<CardModel> GroupList)
        {
            this.VMVCIGroupList.GroupName = GroupName;
            this.VMVCIGroupList.UpdateCategoryList(GroupList);
        }

    }
}
