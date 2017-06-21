using CollectorsFrontEnd.InterfaceModels;
using CollectorsFrontEnd.StoreFrontSupport;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace CollectorsFrontEnd.Views.ViewComponents
{
    public class VMVCIGroupList : ViewModel
    {
        public string GroupName { get; set; }
        public ObservableCollection<string> CategoryGroup { get; set; } = new ObservableCollection<string>() { };

        public VMVCIGroupList(UserControl Model) : base(Model) { }

        public void UpdateCategoryList(IEnumerable<CardModel> alstNew)
        {
            int iCmc = -1;
            string szCmc = alstNew.First().GetAttr("cmc");
            if (!int.TryParse(szCmc, out iCmc) && szCmc != "") { return; }

            CategoryGroup.Clear();
            foreach (CardModel oCardModel in alstNew)
            {
                int newCmc;
                szCmc = oCardModel.GetAttr("cmc");
                if (int.TryParse(szCmc, out newCmc))
                {
                    if (newCmc != iCmc)
                    {
                        if (iCmc != -1)
                        {
                            // This is a space between cmcs
                            ListViewItem blankItem = new ListViewItem();
                            blankItem.Content = "";
                            //blankItem.MouseLeave += eItemList_MouseLeave;
                            //blankItem.MouseEnter += eItemList_MouseMove;
                            //CategoryGroup.Add(blankItem);
                        }

                        iCmc = newCmc;
                    }
                }
                //ListViewItem newItem = new ListViewItem();
                //newItem.Content = oCardModel.GetIdealIdentifier();
                //newItem.DataContext = this;
                //newItem.MouseLeave += eItemList_MouseLeave;
                //newItem.MouseEnter += eItemList_MouseMove;
                CategoryGroup.Add(oCardModel.GetIdealIdentifier());
            }
        }

        #region Private Functions
        private int CompareCMC(CardModel x, CardModel y)
        {
            int xCmc;
            int yCmc;
            string xszCmc = x.GetAttr("cmc");
            string yszCmc = y.GetAttr("cmc");

            bool xNum = int.TryParse(xszCmc, out xCmc);
            bool yNum = int.TryParse(yszCmc, out yCmc);

            if (xNum && yNum)
            {
                if (xCmc == yCmc)
                {
                    return 0;
                }
                else
                {
                    return (xCmc > yCmc) ? 1 : -1;
                }
            }
            else if (xNum && !yNum)
            {
                return 1;
            }
            else if (!xNum && yNum)
            {
                return -1;
            }
            else
            {
                return 0;
            }
        }
        #endregion
    }
}
