using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
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
using System.Windows.Threading;

namespace ExConsole2
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly CancellationTokenSource _cancellation = new();

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool AllocConsole();

        public MainWindow()
        {
            AllocConsole();
            InitializeComponent();

            // for (int i = 0; i < 1000; ++i)
            // {
            //     addLogging("ここに表示されるテキストは選択してコピーできます 🌴");
            // }
            ConnectToPipe(_cancellation.Token);
        }

        public void addLogging(string text)
        {
            var textBox = new TextBox()
            {
                Text = text,
                IsReadOnly = true,
                TextWrapping = TextWrapping.Wrap,
                Margin = new Thickness(8, 2, 8, 2),
            };
            stackPanel.Children.Add(textBox);
        }

        private async Task ConnectToPipe(CancellationToken cancellation)
        {
            while (true)
            {
                addLogging("Waiting...");
                string? line = await Task.Run(() => Console.In.ReadLine(), cancellation);
                if (line == null) continue;

                Dispatcher.Invoke(() =>
                {
                    // ここでWPFアプリケーションのUIを更新
                    addLogging(line);
                });
            }
        }

        private void onClosed(object? sender, EventArgs e)
        {
            _cancellation.Cancel();
        }
    }
}