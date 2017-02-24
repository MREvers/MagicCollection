using CollectorsFrontEnd.InterfaceModels;
using System;
using System.Collections.Generic;
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
    /// Interaction logic for CompSubAmountInterchanger.xaml
    /// </summary>
    public partial class CompSubAmountInterchanger : UserControl, IComponent
    {
        public List<CompSubAmountChanger> LstGeneralizations
        {
            get
            {
                return DataModel.LstGeneralizations;
            }
        }

        public string CardName { get; set; }

        public class AmountInterchangerModel : IDataModel
        {
            public List<CardModel> Copies;
            public List<CompSubAmountChanger> LstGeneralizations = new List<CompSubAmountChanger>();
        }

        public AmountInterchangerModel DataModel = new AmountInterchangerModel();

        public event ComponentEvent UnhandledEvent;

        // Expects a list of IDENTICAL CARDS (Not included metatags)
        public CompSubAmountInterchanger(List<CardModel> aLstCopies)
        {
            InitializeComponent();
            DataContext = this;
            DataModel.Copies = aLstCopies;
            CardName = aLstCopies[0].CardName;
            buildList();
        }

        public void buildList()
        {
            foreach (CardModel CM in DataModel.Copies)
            {
                string szGeneralization = CM.GetMetaTag("Generalization");
                szGeneralization = (szGeneralization == "" ? "Main" : szGeneralization);

                CompSubAmountChanger oTemp = new CompSubAmountChanger(szGeneralization, CM.Amount);
                DataModel.LstGeneralizations.Add(oTemp);

            }
        }

        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            throw new NotImplementedException();
        }

        public IDataModel GetDataModel()
        {
            return DataModel;
        }

        private void BtnCancel_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "Close");
        }

        private void BtnOK_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "OK");
        }
    }
}
