﻿using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
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

namespace CollectorsFrontEnd.Interfaces.Subs
{

    /// <summary>
    /// Interaction logic for Module_GroupView.xaml
    /// </summary>
    public partial class Module_GroupView : UserControl
    {
        #region Data Binding

        public ObservableCollection<Module_CardGroupList> LstGroups { get; set; }

        #endregion

        public event SizeChangedEventHandler SizeChanged;

        public Module_GroupView()
        {
            InitializeComponent();
            DataContext = this;

        }

        private void eMainDisplay_Resize(object sender, SizeChangedEventArgs e)
        {
            if (SizeChanged != null)
            {
                SizeChanged(sender, e);
            }
        }
    }
}
