#include <atomic>
#include <cassert>
#include <print>
#include <thread>

std::atomic<bool> balanceDebited{false};
std::atomic<bool> payoutRecorded{false};

bool gameSawNoPayout = false;
bool accountingSawNoDebit = false;

namespace order{
    void GameThread()
    {
        balanceDebited.store(true, std::memory_order_seq_cst);
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));

        gameSawNoPayout =
            !payoutRecorded.load(std::memory_order_seq_cst);
    }

    void AccountingThread()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        payoutRecorded.store(true, std::memory_order_seq_cst);
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));

        accountingSawNoDebit =
            !balanceDebited.load(std::memory_order_seq_cst);
    }
}

namespace relax{
    void GameThread()
    {
        balanceDebited.store(true, std::memory_order_relaxed);
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));

        gameSawNoPayout =
            !payoutRecorded.load(std::memory_order_relaxed);
    }

    void AccountingThread()
    {
        payoutRecorded.store(true, std::memory_order_relaxed);
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));

        accountingSawNoDebit =
            !balanceDebited.load(std::memory_order_relaxed);
    }
}

int main()
{
    {
        std::println("Start seq_cst");
        std::thread game{order::GameThread};
        std::thread accounting{order::AccountingThread};

        game.join();
        accounting.join();

        std::println("game saw no payout: {}",
                    gameSawNoPayout);

        std::println("accounting saw no debit: {}",
                    accountingSawNoDebit);

        assert(!(gameSawNoPayout && accountingSawNoDebit));
    }

    {
        std::println("relaxed");
        std::thread game{relax::GameThread};
        std::thread accounting{relax::AccountingThread};

        game.join();
        accounting.join(); std::println("game saw no payout: {}", gameSawNoPayout);
        std::println("accounting saw no debit: {}", accountingSawNoDebit);

        // assert(!(gameSawNoPayout && accountingSawNoDebit));
    }
}