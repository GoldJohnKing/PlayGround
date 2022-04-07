using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
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

namespace SimpleWpfWindow
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        System.Diagnostics.Process process = new();
        bool processStarted = false;

        public MainWindow()
        {
            InitializeComponent();

            System.Threading.Thread thread = new(new System.Threading.ThreadStart(ThreadProc));
        }

        private void ThreadProc()
        {
            System.Threading.Thread.Sleep(15000);
            if (processStarted)
                return;

            process.StartInfo.UseShellExecute = false;
            process.StartInfo.FileName = ProcessPath.Text;
            process.Start();

            processStarted = true;
        }

        private void RunButton_Click(object sender, RoutedEventArgs e)
        {
            if (processStarted)
                return;

            process.StartInfo.UseShellExecute = false;
            process.StartInfo.FileName = ProcessPath.Text;
            process.Start();

            processStarted = true;
        }

        private void KillButton_Click(object sender, RoutedEventArgs e)
        {
            if (!processStarted)
                return;

            process.Kill();
            process.Close();

            processStarted = false;
        }
    }
}
